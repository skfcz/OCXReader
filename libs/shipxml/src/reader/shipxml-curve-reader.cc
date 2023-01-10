/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
 ***************************************************************************
 *   Copyright (c) 2022, Carsten Zerbst (carsten.zerbst@groy-groy.de)      *
 *   Copyright (c) 2022, Paul Buechner                                     *
 *                                                                         *
 *   This file is part of the OCXReader library.                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   version 2.1 as published by the Free Software Foundation.             *
 *                                                                         *
 ***************************************************************************/

#include "shipxml/internal/shipxml-curve-reader.h"

#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeLine.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <cmath>
#include <gp_Pnt.hxx>
#include <magic_enum.hpp>

#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-log.h"

namespace shipxml {

shipxml::AMCurve ReadCurve(LDOM_Element const& curveRootN, MajorPlane const& mp,
                           gp_Dir const& normal) {
  auto meta = ocx::helper::GetOCXMeta(curveRootN);

  auto crv = AMCurve(ToAMSystem(normal));

  LDOM_Node childN = curveRootN.getFirstChild();
  while (childN != nullptr) {
    std::vector<ArcSegment> curveSegments;

    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element curveN = (LDOM_Element&)childN;
      std::string curveType = ocx::helper::GetLocalTagName(curveN);
      if (curveType == "CompositeCurve3D") {
        curveSegments = ReadCompositeCurve3D(curveN, crv);
      } else if (curveType == "Ellipse3D") {
        // By default, Ellipse3D should be a closed curve
        curveSegments = ReadEllipse3D(curveN, crv);
        break;
      } else if (curveType == "CircumCircle3D") {
        // By default, CircumCircle3D should be a closed curve
        curveSegments = ReadCircumCircle3D(curveN, crv);
        break;
      } else if (curveType == "Circle3D") {
        // By default, Circle3D should be a closed curve
        curveSegments = ReadCircle3D(curveN, crv);
        break;
      } else if (curveType == "CircumArc3D") {
        curveSegments = ReadCircumArc3D(curveN, crv);
      } else if (curveType == "Line3D") {
        curveSegments = ReadLine3D(curveN, crv);
      } else if (curveType == "PolyLine3D") {
        curveSegments = ReadPolyLine3D(curveN, crv);
      } else if (curveType == "NURBS3D") {
        curveSegments = ReadNURBS3D(curveN, crv);
      } else {
        SHIPXML_WARN(
            "Found unsupported curve type {} in curve {} id={} guid={}",
            curveType, meta->name, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      if (curveSegments.empty()) {
        SHIPXML_WARN("Failed to read curve type {} in curve {} id={} guid={}",
                     curveType, meta->name, meta->id, meta->guid)

      } else {
        SHIPXML_INFO("Add #{} arcs", curveSegments.size())
        crv.GetSegments().insert(crv.GetSegments().end(), curveSegments.begin(),
                                 curveSegments.end());
        SHIPXML_INFO("Added #{} arcs", crv.GetSegments().size())
      }
    }
    childN = childN.getNextSibling();
  }

  SHIPXML_INFO("Read curve and split into #{} arcs", crv.GetSegments().size())

  return crv;
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadNURBS3D(const LDOM_Element& nurbs3DN,
                                    AMCurve const& amCurve) {
  auto meta = ocx::helper::GetOCXMeta(nurbs3DN);

  LDOM_Element propsN = ocx::helper::GetFirstChild(nurbs3DN, "NURBSproperties");
  if (propsN.isNull()) {
    SHIPXML_ERROR(
        "No NURBSproperties child node found in NURBS3D with curve id={}",
        meta->id)
    return {};
  }

  int degree{}, numCtrlPoints{}, numKnots{};
  propsN.getAttribute("degree").GetInteger(degree);
  propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPoints);
  propsN.getAttribute("numKnots").GetInteger(numKnots);

  auto form = std::string(propsN.getAttribute("form").GetString());
  bool isRational =
      ocx::utils::stob(propsN.getAttribute("isRational").GetString());

  SHIPXML_DEBUG(
      "Found NURBSproperties: degree={}, numCtrlPoints={}, numKnots={}, "
      "form={}, isRational={}",
      degree, numCtrlPoints, numKnots, form.empty() ? "Unknown" : form,
      isRational)

  // Parse knotVector
  LDOM_Element knotVectorN = ocx::helper::GetFirstChild(nurbs3DN, "KnotVector");
  if (knotVectorN.isNull()) {
    SHIPXML_ERROR("No KnotVector child node found in NURBS3D with curve id={}",
                  meta->id)
    return {};
  }
  auto knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
  ocx::helper::KnotMults kn =
      ocx::helper::ParseKnotVector(knotVectorS, numKnots);
  if (kn.IsNull) {
    SHIPXML_ERROR("Failed to parse KnotVector in NURBS3D with curve id={}",
                  meta->id)
    return {};
  }

  // Parse control Points
  LDOM_Element controlPtListN =
      ocx::helper::GetFirstChild(nurbs3DN, "ControlPtList");
  if (controlPtListN.isNull()) {
    SHIPXML_ERROR(
        "No ControlPtList child node found in NURBS3D with curve id={}",
        meta->id)
    return {};
  }
  ocx::helper::PolesWeightsCurve pw =
      ocx::helper::ParseControlPointsCurve(controlPtListN, numCtrlPoints);
  if (pw.IsNull) {
    SHIPXML_ERROR("Failed to parse ControlPtList in NURBS3D with curve id={}",
                  meta->id)
    return {};
  }

  // Create the curve
  Handle(Geom_BSplineCurve) curve =
      new Geom_BSplineCurve(pw.poles, pw.weights, kn.knots, kn.mults, degree,
                            Standard_False, isRational);

  // TODO: replace by lookup of calculated curve

  double uStart = kn.knots.First();
  double uEnd = kn.knots.Last();
  int deltas = (numCtrlPoints * 3);
  double deltaU = (uEnd - uStart) / deltas;

  SHIPXML_DEBUG("u [{}, {}], delta u {}, #{}", uStart, uEnd, deltaU, deltas);

  // precalculate the points for performance reasons
  auto points = std::vector<gp_Pnt>();
  gp_Pnt firstPoint;
  curve->D0(uStart, firstPoint);
  points.push_back(firstPoint);

  double tolerance = 0.002;  // we assume 2mm is ok
  double u = uStart + deltaU;
  while (u < uEnd) {
    gp_Pnt point;
    curve->D0(u, point);

    // if the distance to the last point is not > tolerance, skip it
    if (points.back().Distance(point) > tolerance) {
      points.push_back(point);
    }
    u += deltaU;
  }
  gp_Pnt lastPoint;
  curve->D0(uEnd, lastPoint);
  points.push_back(lastPoint);

  SHIPXML_DEBUG("created #{} points", points.size());

  //    for ( int i = 0; i < points.size();i++) {
  //      SHIPXML_DEBUG( "#{} {}", i, ToString( Convert(points.at(i))))
  //    }

  auto segments = std::vector<ArcSegment>();

  // now we want to find the arcs which span a maximal amount of points
  // we expect that a line from two points is always a valid solution
  int currentStart = 0;
  int lastValidEnd = currentStart + 1;
  bool lastValidIsArc = false;
  int currentEnd = lastValidEnd + 1;

  while (currentEnd < points.size()) {
    // SHIPXML_DEBUG("check from #{} to #{}", currentStart, currentEnd)
    //  we build a circle from three points
    auto arcStart = points.at(currentStart);
    auto arcEnd = points.at(currentEnd);
    int iItm = (currentEnd - currentStart) < 3
                   ? currentStart + 1
                   : (int)ceil((currentStart + currentEnd) / 2.0);
    auto arcItm = points.at(iItm);

    //    SHIPXML_DEBUG("start {}, end {}, onArc {}",
    //              ToString(Convert(arcStart)),
    //              ToString(Convert(arcEnd)),
    //              ToString(Convert(arcItm)));

    bool isValid = true;
    bool isArc = false;

    // we spare the effort of using GProp_PEquation and directly use
    // GC_MakeArcOfCircle to check and create
    GC_MakeArcOfCircle mkacc(arcStart, arcEnd, arcItm);
    if (mkacc.IsDone()) {
      Handle(Geom_TrimmedCurve) arc = mkacc.Value();
      Handle(Geom_Curve) baseCurve = arc->BasisCurve();
      Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(baseCurve);

      // for a circle using brute code instead of BRepExtrema_DistShapeShape is
      // much faster
      gp_Pnt center = circle->Location();
      double radius = circle->Radius();

      // SHIPXML_DEBUG("created circle at {},r {}", ToString(Convert(center)),
      // radius)

      for (int i = currentStart + 1; i < currentEnd; i++) {
        gp_Pnt pointToCheck = points.at(i);

        if (abs(center.Distance(pointToCheck) - radius) > tolerance) {
          // ok, we exceeded the distance, this solution is not valid;
          SHIPXML_DEBUG("check failed at {} {} {}", center.X(), center.Y(),
                        center.Z())
          isValid = false;
          break;
        }
      }

      isArc = true;
    } else {
      // points most probably have been collinear, so we check using a line
      // for a line using brute code instead of BRepExtrema_DistShapeShape is
      // much faster see
      // https://math.stackexchange.com/questions/1905533/find-perpendicular-distance-from-point-to-line-in-3d
      GC_MakeLine mkln(arcStart, arcEnd);
      if (mkln.IsDone()) {
        SHIPXML_DEBUG("check as line")

        //   vec3 d = (C - B) / C.distance(B);
        auto d = gp_Vec(arcStart, arcEnd);
        d.Normalize();
        for (int i = currentStart + 1; i < currentEnd; i++) {
          gp_Pnt pointToCheck = points.at(i);

          // vec3 v = A - B;
          auto v = gp_Vec(arcStart, pointToCheck);
          // double t = v.dot(d);
          double t = v.Dot(d);
          // vec3 P = B + t * d;
          auto P = gp_Pnt(arcEnd.X() + t * d.X(), arcEnd.Y() + t * d.Y(),
                          arcEnd.Y() + t * d.Y());
          if (P.Distance(pointToCheck) > tolerance) {
            // check failed
            SHIPXML_DEBUG("check failed at [{} {} {}]", pointToCheck.X(),
                          pointToCheck.Y(), pointToCheck.Z())
            isValid = false;
            break;
          }
        }  // end of line check loop
        isArc = false;
      } else {
        SHIPXML_ERROR("failed to create a line from [{} {} {}] to [{} {} {}]",
                      arcStart.X(), arcStart.Y(), arcStart.Z(), arcEnd.X(),
                      arcEnd.Y(), arcEnd.Z())
        isValid = false;
      }
    }  // end of arc/line check
    if (isValid) {
      lastValidEnd = currentEnd;
      lastValidIsArc = isArc;
      currentEnd++;
      // special handling for the last arc in case that lastValidEnd == last
      // point
      // SHIPXML_DEBUG("check passed at #{}, next check for #{},
      // {}",lastValidEnd, currentEnd, isArc ? " as arc" :  "as line")
      if (lastValidIsArc != points.size() - 1) {
        continue;
      }
    }
    // fall back to the previous solution
    SHIPXML_DEBUG("check failed, go back to #{} to #{}, create {}",
                  currentStart, lastValidEnd, lastValidIsArc ? "arc" : "line")
    gp_Pnt foundStart = points.at(currentStart);
    gp_Pnt end = points.at(lastValidEnd);

    // SHIPXML_DEBUG("    start {}, end {}", ToString(Convert(foundStart)),
    // ToString(Convert(end)))

    if (lastValidIsArc) {
      iItm = (lastValidEnd - currentStart) < 3
                 ? currentStart + 1
                 : (int)ceil((currentStart + lastValidEnd) / 2.0);
      arcItm = points.at(iItm);

      // SHIPXML_DEBUG("    found onArc #{}, {}", iItm,
      // ToString(Convert(arcItm)))

      GC_MakeArcOfCircle mkfacc(foundStart, end, arcItm);
      if (!mkfacc.IsDone()) {
        SHIPXML_ERROR("    failed to create arc")
      }

      Handle(Geom_Curve) baseCurve = mkfacc.Value()->BasisCurve();
      Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(baseCurve);

      // TODO: check withershins

      auto arcSegment =
          ArcSegment(arcStart, arcEnd, arcItm, circle->Location(), true);
      segments.push_back(arcSegment);

    } else {
      segments.emplace_back(foundStart, end);
    }

    // now continue from last valid solution
    currentStart = lastValidEnd;
    lastValidEnd = currentStart + 1;
    lastValidIsArc = false;
    currentEnd = lastValidEnd + 1;

    if (currentEnd >= points.size()) {
      break;
    }

  }  // end of while loop

  if (lastValidEnd + 1 < points.size()) {
    SHIPXML_DEBUG("add closing line from #{} to #{}", currentStart,
                  points.size() - 1);
    auto arcStart = points.at(currentStart);
    auto const& arcEnd = points.at(points.size() - 1);
    segments.emplace_back(arcStart, arcEnd);
  }

  SHIPXML_DEBUG("created #{} arcs", segments.size());

  return segments;
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadPolyLine3D(const LDOM_Element& crvEL,
                                       AMCurve const& curve) {
  SHIPXML_ERROR("ReadPolyLine3D not implemented")
  return {};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadLine3D(const LDOM_Element& lineEL,
                                   AMCurve const& curve) {
  auto meta = ocx::helper::GetOCXMeta(lineEL);

  LDOM_Element startN = ocx::helper::GetFirstChild(lineEL, "StartPoint");
  if (startN.isNull()) {
    SHIPXML_ERROR(
        "No StartPoint child node found in Line3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element endN = ocx::helper::GetFirstChild(lineEL, "EndPoint");
  if (endN.isNull()) {
    SHIPXML_ERROR(
        "No EndPoint child node found in Line3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt start = ocx::helper::ReadPoint(startN);
  gp_Pnt end = ocx::helper::ReadPoint(endN);

  return {ArcSegment(start, end)};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadCircumArc3D(const LDOM_Element& circleN,
                                        AMCurve const& curve) {
  auto meta = ocx::helper::GetOCXMeta(circleN);

  LDOM_Element startN = ocx::helper::GetFirstChild(circleN, "StartPoint");
  if (startN.isNull()) {
    SHIPXML_ERROR(
        "No StartPoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element intN = ocx::helper::GetFirstChild(circleN, "IntermediatePoint");
  if (intN.isNull()) {
    SHIPXML_ERROR(
        "No IntermediatePoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element endN = ocx::helper::GetFirstChild(circleN, "EndPoint");
  if (endN.isNull()) {
    SHIPXML_ERROR(
        "No EndPoint child node found in CircumArc3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt start = ocx::helper::ReadPoint(startN);
  gp_Pnt intermediate = ocx::helper::ReadPoint(intN);
  gp_Pnt end = ocx::helper::ReadPoint(endN);

  Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, end, intermediate);
  Handle(Geom_Curve) baseCurve = arc->BasisCurve();
  Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(baseCurve);
  gp_Pnt center = circle->Location();

  auto c2s = gp_Vec(center, start);
  auto c2e = gp_Vec(center, end);
  gp_Vec crossed = c2s.Crossed(c2e);

  // ... when seen from fore, above or starboard
  gp_Vec reference{};
  switch (curve.GetSystem()) {
    case AMSystem::XY:
      reference = gp_Vec(0, 0, 1);
      break;
    case AMSystem::YZ:
      reference = gp_Vec(1, 0, 0);
      break;
    case AMSystem::XZ:
      reference = gp_Vec(0, -1, 0);
      break;
    default:
      SHIPXML_ERROR("Undefined system: {}",
                    magic_enum::enum_name(curve.GetSystem()))
      return {};
  }
  bool withershins = crossed.Angle(reference) > 0;

  // TODO: check if alpha > PI/2, then split into to ArcSegments

  return {ArcSegment(start, end, intermediate, center, withershins)};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadCircle3D(const LDOM_Element& circleN,
                                     AMCurve const& curve) {
  // TODO: no test data available, check it works as expected

  auto meta = ocx::helper::GetOCXMeta(circleN);

  LDOM_Element startN = ocx::helper::GetFirstChild(circleN, "StartPoint");
  if (startN.isNull()) {
    SHIPXML_ERROR(
        "No StartPoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element intN = ocx::helper::GetFirstChild(circleN, "IntermediatePoint");
  if (intN.isNull()) {
    SHIPXML_ERROR(
        "No IntermediatePoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element endN = ocx::helper::GetFirstChild(circleN, "EndPoint");
  if (endN.isNull()) {
    SHIPXML_ERROR(
        "No EndPoint child node found in CircumArc3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt start = ocx::helper::ReadPoint(startN);
  gp_Pnt intermediate = ocx::helper::ReadPoint(intN);
  gp_Pnt end = ocx::helper::ReadPoint(endN);

  Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, end, intermediate);
  Handle(Geom_Curve) baseCurve = arc->BasisCurve();
  Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(baseCurve);
  gp_Pnt center = circle->Location();

  // Check if the circle is wither-shins when seen from fore, above or starboard
  gp_Vec reference{};
  switch (curve.GetSystem()) {
    case AMSystem::XY:
      reference = gp_Vec(0, 0, 1);
      break;
    case AMSystem::YZ:
      reference = gp_Vec(1, 0, 0);
      break;
    case AMSystem::XZ:
      reference = gp_Vec(0, -1, 0);
      break;
    default:
      SHIPXML_ERROR("Undefined system: {}",
                    magic_enum::enum_name(curve.GetSystem()))
      return {};
  }
  auto axis = circle->Axis();

  double uStart = 0.0;
  double uIm1 = M_PI / 3.0;
  double uEnd = M_PI;
  double uIm2 = M_PI * 4.0 / 3.0;
  if (axis.Direction().Angle(reference) > 45.0 / 180.0 * M_PI) {
    // swap direction
    uStart = circle->ReversedParameter(uStart);
    uIm1 = circle->ReversedParameter(uIm1);
    uEnd = circle->ReversedParameter(uEnd);
    uIm2 = circle->ReversedParameter(uIm2);
  }
  gp_Pnt arcStart;
  circle->D0(uStart, arcStart);

  gp_Pnt arcEnd;
  circle->D0(uEnd, arcEnd);

  gp_Pnt arcIm1;
  circle->D0(uIm1, arcIm1);

  gp_Pnt arcIm2;
  circle->D0(uIm2, arcIm2);

  return {ArcSegment(arcStart, arcEnd, arcIm1, center, true),
          ArcSegment(arcEnd, arcStart, arcIm2, center, true)};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadCircumCircle3D(const LDOM_Element& crvEL,
                                           AMCurve const& curve) {
  SHIPXML_ERROR("ReadCircumCircle3D is not implemented")
  return {};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadEllipse3D(const LDOM_Element& crvEL,
                                      AMCurve const& curve) {
  SHIPXML_ERROR("ReadEllipse3D is not implemented")
  return {};
}

//-----------------------------------------------------------------------------

std::vector<ArcSegment> ReadCompositeCurve3D(const LDOM_Element& crvEL,
                                             AMCurve const& curve) {
  SHIPXML_ERROR("ReadCompositeCurve3D is not implemented")
  return {};
}

}  // namespace shipxml
