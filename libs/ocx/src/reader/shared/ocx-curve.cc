/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

#include "ocx/internal/ocx-curve.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include "ocx/internal/ocx-utils.h"
#include "ocx/ocx-helper.h"

namespace ocx::reader::shared::curve {

TopoDS_Wire ReadCurve(LDOM_Element const &curveRootN) {
  auto meta = ocx::helper::GetOCXMeta(curveRootN);

  auto wireBuilder = BRepBuilderAPI_MakeWire();

  LDOM_Node childN = curveRootN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto edge = TopoDS_Shape();

      LDOM_Element curveN = (LDOM_Element &)childN;
      std::string curveType = ocx::helper::GetLocalTagName(curveN);
      if (curveType == "CompositeCurve3D") {
        edge = ReadCompositeCurve3D(curveN);
      } else if (curveType == "Ellipse3D") {
        // By default, Ellipse3D should be a closed curve
        return ReadEllipse3D(curveN);
      } else if (curveType == "CircumCircle3D") {
        // By default, CircumCircle3D should be a closed curve
        return ReadCircumCircle3D(curveN);
      } else if (curveType == "Circle3D") {
        // By default, Circle3D should be a closed curve
        return ReadCircle3D(curveN);
      } else if (curveType == "CircumArc3D") {
        edge = ReadCircumArc3D(curveN);
      } else if (curveType == "Line3D") {
        edge = ReadLine3D(curveN);
      } else if (curveType == "PolyLine3D") {
        edge = ReadPolyLine3D(curveN);
      } else if (curveType == "NURBS3D") {
        edge = ReadNURBS3D(curveN);
      } else {
        OCX_WARN("Found unsupported curve type {} in curve {} id={} guid={}",
                 curveType, meta->name, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      if (edge.IsNull()) {
        OCX_WARN("Failed to read curve type {} in curve {} id={} guid={}",
                 curveType, meta->name, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      switch (edge.ShapeType()) {
        case TopAbs_WIRE:
          // Return immediately if the shape is a wire
          return TopoDS::Wire(edge);
        case TopAbs_EDGE:
          // Add the edge to the wireBuilder
          wireBuilder.Add(TopoDS::Edge(edge));
          break;
        default:
          OCX_WARN(
              "Received unsupported shape type {} in curve {} id={} guid={}",
              edge.ShapeType(), meta->name, meta->id, meta->guid)
          break;
      }
    }
    childN = childN.getNextSibling();
  }

  wireBuilder.Build();
  if (!wireBuilder.IsDone()) {
    OCX_ERROR("Failed to build wire for curve {} id={} guid={}", meta->name,
              meta->id, meta->guid)
    return {};
  }

  return wireBuilder.Wire();
}

namespace {

TopoDS_Shape ReadCompositeCurve3D(LDOM_Element const &curveColN) {
  auto meta = ocx::helper::GetOCXMeta(curveColN);

  auto wireBuilder = BRepBuilderAPI_MakeWire();

  LDOM_Node childN = curveColN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto edge = TopoDS_Shape();

      LDOM_Element curveN = (LDOM_Element &)childN;
      std::string curveType = ocx::helper::GetLocalTagName(curveN);
      if (curveType == "Line3D") {
        edge = ReadLine3D(curveN);
      } else if (curveType == "CircumArc3D") {
        edge = ReadCircumArc3D(curveN);
      } else if (curveType == "NURBS3D") {
        edge = ReadNURBS3D(curveN);
      } else if (curveType == "PolyLine3D") {
        edge = ReadPolyLine3D(curveN);
      } else {
        OCX_ERROR(
            "Found unsupported curve type {} in CompositeCurve3D with curve "
            "id={} guid={}",
            curveType, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      if (edge.IsNull()) {
        OCX_ERROR(
            "Failed to read curve type {} in CompositeCurve3D with curve id={} "
            "guid={}",
            curveType, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      switch (edge.ShapeType()) {
        case TopAbs_WIRE:
          // Return immediately if the shape is a wire
          return TopoDS::Wire(edge);
        case TopAbs_EDGE:
          // Add the edge to the wireBuilder
          wireBuilder.Add(TopoDS::Edge(edge));
          break;
        default:
          OCX_ERROR(
              "Received unsupported shape type {} in CompositeCurve3D with "
              "curve id={} guid={}",
              edge.ShapeType(), meta->id, meta->guid)
          break;
      }
    }
    childN = childN.getNextSibling();
  }

  wireBuilder.Build();
  if (!wireBuilder.IsDone()) {
    OCX_ERROR("Failed to build wire for CompositeCurve3D {} id={} guid={}",
              meta->name, meta->id, meta->guid)
    return {};
  }

  return wireBuilder.Wire();
}

TopoDS_Wire ReadEllipse3D(LDOM_Element const &ellipseN) {
  auto meta = ocx::helper::GetOCXMeta(ellipseN);

  LDOM_Element centerN = ocx::helper::GetFirstChild(ellipseN, "Center");
  if (centerN.isNull()) {
    OCX_ERROR(
        "No Center child node found in Ellipse3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element majorDiaN =
      ocx::helper::GetFirstChild(ellipseN, "MajorDiameter");
  if (majorDiaN.isNull()) {
    OCX_ERROR(
        "No MajorDiameter child node found in Ellipse3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element minorDiaN =
      ocx::helper::GetFirstChild(ellipseN, "MinorDiameter");
  if (majorDiaN.isNull()) {
    OCX_ERROR(
        "No MinorDiameter child node found in Ellipse3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element majorAxisN = ocx::helper::GetFirstChild(ellipseN, "MajorAxis");
  if (majorAxisN.isNull()) {
    OCX_ERROR(
        "No MajorAxis child node found in Ellipse3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element minorAxisN = ocx::helper::GetFirstChild(ellipseN, "MinorAxis");
  if (minorAxisN.isNull()) {
    OCX_ERROR(
        "No MinorAxis child node found in Ellipse3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt center = ocx::helper::ReadPoint(centerN);
  double dMajor = ocx::helper::ReadDimension(majorDiaN);
  double dMinor = ocx::helper::ReadDimension(minorDiaN);
  gp_Dir majorAxis = ocx::helper::ReadDirection(majorAxisN);
  gp_Dir minorAxis = ocx::helper::ReadDirection(minorAxisN);
  gp_Dir normal = majorAxis.Crossed(minorAxis);

  auto cosys = gp_Ax2(center, normal, majorAxis);
  cosys.SetDirection(majorAxis);

  Handle(Geom_Ellipse) ellipse =
      new Geom_Ellipse(cosys, dMajor / 2.0, dMinor / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(ellipse);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire ReadCircumCircle3D(LDOM_Element const &circleN) {
  auto meta = ocx::helper::GetOCXMeta(circleN);

  LDOM_Element positionsN = ocx::helper::GetFirstChild(circleN, "Positions");
  if (positionsN.isNull()) {
    OCX_ERROR(
        "No Positions child node found in CircumCircle3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt p0{}, p1{}, p2{};

  int pointCnt = 0;
  LDOM_Node childN = positionsN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element pointN = (LDOM_Element &)childN;

      if (ocx::helper::GetLocalTagName(pointN) != "Point3D") {
        OCX_ERROR(
            "Found unsupported child node {} in CircumCircle3D/Positions with "
            "curve id={} guid={}",
            ocx::helper::GetLocalTagName(pointN), meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      switch (pointCnt) {
        case 0:
          p0 = ocx::helper::ReadPoint(pointN);
          break;
        case 1:
          p1 = ocx::helper::ReadPoint(pointN);
          break;
        case 2:
          p2 = ocx::helper::ReadPoint(pointN);
          break;
        default:
          OCX_ERROR(
              "Found more than 3 points in CircumCircle3D/Positions with curve "
              "id={} guid={}",
              meta->id, meta->guid)
          return {};
      }
      pointCnt++;
    }
    childN = childN.getNextSibling();
  }

  Handle(Geom_Circle) circle = GC_MakeCircle(p0, p1, p2);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire ReadCircle3D(LDOM_Element const &circleN) {
  auto meta = ocx::helper::GetOCXMeta(circleN);

  LDOM_Element centerN = ocx::helper::GetFirstChild(circleN, "Center");
  if (centerN.isNull()) {
    OCX_ERROR("No Center child node found in Circle3D with curve id={} guid={}",
              meta->id, meta->guid)
    return {};
  }

  LDOM_Element diaN = ocx::helper::GetFirstChild(circleN, "Diameter");
  if (diaN.isNull()) {
    OCX_ERROR(
        "No Diameter child node found in Circle3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element normalN = ocx::helper::GetFirstChild(circleN, "Normal");
  if (normalN.isNull()) {
    OCX_ERROR("No Normal child node found in Circle3D with curve id={} guid={}",
              meta->id, meta->guid)
    return {};
  }

  gp_Pnt center = ocx::helper::ReadPoint(centerN);
  double diameter = ocx::helper::ReadDimension(diaN);
  gp_Dir normal = ocx::helper::ReadDirection(normalN);

  auto cosys = gp_Ax2(center, normal);

  Handle(Geom_Circle) circle = new Geom_Circle(cosys, diameter / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Edge ReadCircumArc3D(LDOM_Element const &circleN) {
  auto meta = ocx::helper::GetOCXMeta(circleN);

  LDOM_Element startN = ocx::helper::GetFirstChild(circleN, "StartPoint");
  if (startN.isNull()) {
    OCX_ERROR(
        "No StartPoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element intN = ocx::helper::GetFirstChild(circleN, "IntermediatePoint");
  if (intN.isNull()) {
    OCX_ERROR(
        "No IntermediatePoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element endN = ocx::helper::GetFirstChild(circleN, "EndPoint");
  if (endN.isNull()) {
    OCX_ERROR(
        "No EndPoint child node found in CircumArc3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt start = ocx::helper::ReadPoint(startN);
  gp_Pnt intermediate = ocx::helper::ReadPoint(intN);
  gp_Pnt end = ocx::helper::ReadPoint(endN);

  Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, intermediate, end );

  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Edge ReadLine3D(LDOM_Element const &lineN) {
  auto meta = ocx::helper::GetOCXMeta(lineN);

  LDOM_Element startN = ocx::helper::GetFirstChild(lineN, "StartPoint");
  if (startN.isNull()) {
    OCX_ERROR(
        "No StartPoint child node found in Line3D with curve id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element endN = ocx::helper::GetFirstChild(lineN, "EndPoint");
  if (endN.isNull()) {
    OCX_ERROR("No EndPoint child node found in Line3D with curve id={} guid={}",
              meta->id, meta->guid)
    return {};
  }

  gp_Pnt start = ocx::helper::ReadPoint(startN);
  gp_Pnt end = ocx::helper::ReadPoint(endN);

  Handle(Geom_TrimmedCurve) arc = GC_MakeSegment(start, end);

  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Shape ReadPolyLine3D(LDOM_Element const &curveN) {
  OCX_ERROR("PolyLine3D not implemented yet")
  return {};
}

TopoDS_Shape ReadNURBS3D(LDOM_Element const &nurbs3DN) {
  auto meta = ocx::helper::GetOCXMeta(nurbs3DN);

  LDOM_Element propsN = ocx::helper::GetFirstChild(nurbs3DN, "NURBSproperties");
  if (propsN.isNull()) {
    OCX_ERROR("No NURBSproperties child node found in NURBS3D with curve id={}",
              meta->id)
    return {};
  }

  int degree{}, numCtrlPoints{}, numKnots{};
  propsN.getAttribute("degree").GetInteger(degree);
  propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPoints);
  propsN.getAttribute("numKnots").GetInteger(numKnots);

  auto form = std::string(propsN.getAttribute("form").GetString());
  bool isRational = utils::stob(propsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found NURBSproperties: degree={}, numCtrlPoints={}, numKnots={}, "
      "form={}, isRational={}",
      degree, numCtrlPoints, numKnots, form.empty() ? "Unknown" : form,
      isRational)

  // Parse knotVector
  LDOM_Element knotVectorN = ocx::helper::GetFirstChild(nurbs3DN, "KnotVector");
  if (knotVectorN.isNull()) {
    OCX_ERROR("No KnotVector child node found in NURBS3D with curve id={}",
              meta->id)
    return {};
  }
  auto knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
  ocx::helper::KnotMults kn =
      ocx::helper::ParseKnotVector(knotVectorS, numKnots);
  if (kn.IsNull) {
    OCX_ERROR("Failed to parse KnotVector in NURBS3D with curve id={}",
              meta->id)
    return {};
  }

  // Parse control Points
  LDOM_Element controlPtListN =
      ocx::helper::GetFirstChild(nurbs3DN, "ControlPtList");
  if (controlPtListN.isNull()) {
    OCX_ERROR("No ControlPtList child node found in NURBS3D with curve id={}",
              meta->id)
    return {};
  }
  ocx::helper::PolesWeightsCurve pw =
      ocx::helper::ParseControlPointsCurve(controlPtListN, numCtrlPoints);
  if (pw.IsNull) {
    OCX_ERROR("Failed to parse ControlPtList in NURBS3D with curve id={}",
              meta->id)
    return {};
  }

  // Create the curve
  Handle(Geom_BSplineCurve) curve =
      new Geom_BSplineCurve(pw.poles, pw.weights, kn.knots, kn.mults, degree,
                            Standard_False, isRational);

  TopoDS_Shape res = BRepBuilderAPI_MakeEdge(curve);

  if (curve->IsClosed()) {
    res = BRepBuilderAPI_MakeWire(TopoDS::Edge(res));
  }

  return res;
}

}  // namespace

}  // namespace ocx::reader::shared::curve
