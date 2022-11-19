// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-curve-reader.h"

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
#include <memory>
#include <utility>

#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-util.h"

namespace ocx {

TopoDS_Wire OCXCurveReader::ReadCurve(LDOM_Element const &curveRootN) const {
  char const *name = curveRootN.getAttribute("name").GetString();
  char const *guid = curveRootN.getAttribute(ctx->OCXGUIDRef()).GetString();
  char const *id = curveRootN.getAttribute("id").GetString();

  auto wireBuilder = BRepBuilderAPI_MakeWire();

  LDOM_Node childN = curveRootN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto edge = TopoDS_Shape();

      LDOM_Element curveN = (LDOM_Element &)childN;
      std::string curveType = OCXHelper::GetLocalTagName(curveN);
      if (curveType == "CompositeCurve3D") {
        edge = ReadCompositeCurve3D(curveN, id, guid);
      } else if (curveType == "Ellipse3D") {
        // By default, Ellipse3D should be a closed curve
        return ReadEllipse3D(curveN, id, guid);
      } else if (curveType == "CircumCircle3D") {
        // By default, CircumCircle3D should be a closed curve
        return ReadCircumCircle3D(curveN, id, guid);
      } else if (curveType == "Circle3D") {
        // By default, Circle3D should be a closed curve
        return ReadCircle3D(curveN, id, guid);
      } else if (curveType == "CircumArc3D") {
        edge = ReadCircumArc3D(curveN, id, guid);
      } else if (curveType == "Line3D") {
        edge = ReadLine3D(curveN, id, guid);
      } else if (curveType == "PolyLine3D") {
        edge = ReadPolyLine3D(curveN, id, guid);
      } else if (curveType == "NURBS3D") {
        edge = ReadNURBS3D(curveN, id, guid);
      } else {
        OCX_WARN("Found unsupported curve type {} in curve {} id={} guid={}",
                 curveType, name, id, guid);
        childN = childN.getNextSibling();
        continue;
      }

      if (edge.IsNull()) {
        OCX_WARN("Failed to read curve type {} in curve {} id={} guid={}",
                 curveType, name, id, guid);
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
              edge.ShapeType(), name, id, guid);
          break;
      }
    }
    childN = childN.getNextSibling();
  }
  return wireBuilder.Wire();
}

TopoDS_Shape OCXCurveReader::ReadCompositeCurve3D(LDOM_Element const &cCurveN,
                                                  std::string_view id,
                                                  std::string_view guid) const {
  auto wireBuilder = BRepBuilderAPI_MakeWire();

  LDOM_Node childN = cCurveN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto edge = TopoDS_Shape();

      LDOM_Element curveN = (LDOM_Element &)childN;
      std::string curveType = OCXHelper::GetLocalTagName(curveN);
      if (curveType == "Line3D") {
        edge = ReadLine3D(curveN, id, guid);
      } else if (curveType == "CircumArc3D") {
        edge = ReadCircumArc3D(curveN, id, guid);
      } else if (curveType == "NURBS3D") {
        edge = ReadNURBS3D(curveN, id, guid);
      } else if (curveType == "PolyLine3D") {
        edge = ReadPolyLine3D(curveN, id, guid);
      } else {
        OCX_ERROR(
            "Found unsupported curve type {} in CompositeCurve3D with curve "
            "id={} guid={}",
            curveType, id, guid);
        childN = childN.getNextSibling();
        continue;
      }

      if (edge.IsNull()) {
        OCX_ERROR(
            "Failed to read curve type {} in CompositeCurve3D with curve id={} "
            "guid={}",
            curveType, id, guid);
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
              edge.ShapeType(), id, guid);
          break;
      }
    }
    childN = childN.getNextSibling();
  }

  return wireBuilder.Wire();
}

TopoDS_Wire OCXCurveReader::ReadEllipse3D(LDOM_Element const &ellipseN,
                                          std::string_view id,
                                          std::string_view guid) const {
  LDOM_Element centerN = OCXHelper::GetFirstChild(ellipseN, "Center");
  if (centerN.isNull()) {
    OCX_ERROR(
        "No Center child node found in Ellipse3D with curve id={} guid={}", id,
        guid);
    return {};
  }

  LDOM_Element majorDiaN = OCXHelper::GetFirstChild(ellipseN, "MajorDiameter");
  if (majorDiaN.isNull()) {
    OCX_ERROR(
        "No MajorDiameter child node found in Ellipse3D with curve id={} "
        "guid={}",
        id, guid);
    return {};
  }

  LDOM_Element minorDiaN = OCXHelper::GetFirstChild(ellipseN, "MinorDiameter");
  if (majorDiaN.isNull()) {
    OCX_ERROR(
        "No MinorDiameter child node found in Ellipse3D with curve id={} "
        "guid={}",
        id, guid);
    return {};
  }

  LDOM_Element majorAxisN = OCXHelper::GetFirstChild(ellipseN, "MajorAxis");
  if (majorAxisN.isNull()) {
    OCX_ERROR(
        "No MajorAxis child node found in Ellipse3D with curve id={} guid={}",
        id, guid);

    return {};
  }

  LDOM_Element minorAxisN = OCXHelper::GetFirstChild(ellipseN, "MinorAxis");
  if (minorAxisN.isNull()) {
    OCX_ERROR(
        "No MinorAxis child node found in Ellipse3D with curve id={} guid={}",
        id, guid);
    return {};
  }

  gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
  double dMajor = OCXHelper::ReadDimension(majorDiaN, ctx);
  double dMinor = OCXHelper::ReadDimension(minorDiaN, ctx);
  gp_Dir majorAxis = OCXHelper::ReadDirection(majorAxisN);
  gp_Dir minorAxis = OCXHelper::ReadDirection(minorAxisN);
  gp_Dir normal = majorAxis.Crossed(minorAxis);

  auto cosys = gp_Ax2(center, normal, majorAxis);
  cosys.SetDirection(majorAxis);

  Handle(Geom_Ellipse) ellipse =
      new Geom_Ellipse(cosys, dMajor / 2.0, dMinor / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(ellipse);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire OCXCurveReader::ReadCircumCircle3D(LDOM_Element const &circleN,
                                               std::string_view id,
                                               std::string_view guid) const {
  LDOM_Element positionsN = OCXHelper::GetFirstChild(circleN, "Positions");
  if (positionsN.isNull()) {
    OCX_ERROR(
        "No Positions child node found in CircumCircle3D with curve id={} "
        "guid={}",
        id, guid);
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

      if (OCXHelper::GetLocalTagName(pointN) != "Point3D") {
        OCX_ERROR(
            "Found unsupported child node {} in CircumCircle3D/Positions with "
            "curve id={} guid={}",
            OCXHelper::GetLocalTagName(pointN), id, guid);
        childN = childN.getNextSibling();
        continue;
      }

      switch (pointCnt) {
        case 0:
          p0 = OCXHelper::ReadPoint(pointN, ctx);
          break;
        case 1:
          p1 = OCXHelper::ReadPoint(pointN, ctx);
          break;
        case 2:
          p2 = OCXHelper::ReadPoint(pointN, ctx);
          break;
        default:
          OCX_ERROR(
              "Found more than 3 points in CircumCircle3D/Positions with curve "
              "id={} guid={}",
              id, guid);
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

TopoDS_Wire OCXCurveReader::ReadCircle3D(LDOM_Element const &circleN,
                                         std::string_view id,
                                         std::string_view guid) const {
  LDOM_Element centerN = OCXHelper::GetFirstChild(circleN, "Center");
  if (centerN.isNull()) {
    OCX_ERROR("No Center child node found in Circle3D with curve id={} guid={}",
              id, guid);
    return {};
  }

  LDOM_Element diaN = OCXHelper::GetFirstChild(circleN, "Diameter");
  if (diaN.isNull()) {
    OCX_ERROR(
        "No Diameter child node found in Circle3D with curve id={} guid={}", id,
        guid);
    return {};
  }

  LDOM_Element normalN = OCXHelper::GetFirstChild(circleN, "Normal");
  if (normalN.isNull()) {
    OCX_ERROR("No Normal child node found in Circle3D with curve id={} guid={}",
              id, guid);
    return {};
  }

  gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
  double diameter = OCXHelper::ReadDimension(diaN, ctx);
  gp_Dir normal = OCXHelper::ReadDirection(normalN);

  auto cosys = gp_Ax2(center, normal);

  Handle(Geom_Circle) circle = new Geom_Circle(cosys, diameter / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Edge OCXCurveReader::ReadCircumArc3D(LDOM_Element const &circleN,
                                            std::string_view id,
                                            std::string_view guid) const {
  LDOM_Element startN = OCXHelper::GetFirstChild(circleN, "StartPoint");
  if (startN.isNull()) {
    OCX_ERROR(
        "No StartPoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        id, guid);
    return {};
  }

  LDOM_Element intN = OCXHelper::GetFirstChild(circleN, "IntermediatePoint");
  if (intN.isNull()) {
    OCX_ERROR(
        "No IntermediatePoint child node found in CircumArc3D with curve id={} "
        "guid={}",
        id, guid);
    return {};
  }

  LDOM_Element endN = OCXHelper::GetFirstChild(circleN, "EndPoint");
  if (endN.isNull()) {
    OCX_ERROR(
        "No EndPoint child node found in CircumArc3D with curve id={} guid={}",
        id, guid);
    return {};
  }

  gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
  gp_Pnt intermediate = OCXHelper::ReadPoint(intN, ctx);
  gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

  Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, end, intermediate);
  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Edge OCXCurveReader::ReadLine3D(LDOM_Element const &lineN,
                                       std::string_view id,
                                       std::string_view guid) const {
  LDOM_Element startN = OCXHelper::GetFirstChild(lineN, "StartPoint");
  if (startN.isNull()) {
    OCX_ERROR(
        "No StartPoint child node found in Line3D with curve id={} guid={}", id,
        guid);
    return {};
  }

  LDOM_Element endN = OCXHelper::GetFirstChild(lineN, "EndPoint");
  if (endN.isNull()) {
    OCX_ERROR("No EndPoint child node found in Line3D with curve id={} guid={}",
              id, guid);
    return {};
  }

  gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
  gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

  Handle(Geom_TrimmedCurve) arc = GC_MakeSegment(start, end);
  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Shape OCXCurveReader::ReadPolyLine3D(LDOM_Element const &curveN,
                                            std::string_view id,
                                            std::string_view guid) const {
  OCX_ERROR("PolyLine3D not implemented yet");
  return {};
}

TopoDS_Shape OCXCurveReader::ReadNURBS3D(LDOM_Element const &nurbs3DN,
                                         std::string_view id,
                                         std::string_view guid) const {
  LDOM_Element propsN = OCXHelper::GetFirstChild(nurbs3DN, "NURBSproperties");
  if (propsN.isNull()) {
    OCX_ERROR(
        "No NURBSproperties child node found in NURBS3D with curve id={} "
        "guid={}",
        id, guid);
    return {};
  }

  int degree{}, numCtrlPoints{}, numKnots{};
  propsN.getAttribute("degree").GetInteger(degree);
  propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPoints);
  propsN.getAttribute("numKnots").GetInteger(numKnots);

  auto form = std::string(propsN.getAttribute("form").GetString());
  bool isRational = stob(propsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found NURBSproperties: degree={}, numCtrlPoints={}, numKnots={}, "
      "form={}, isRational={}",
      degree, numCtrlPoints, numKnots, form.empty() ? "Unknown" : form,
      isRational);

  // Parse knotVector
  LDOM_Element knotVectorN = OCXHelper::GetFirstChild(nurbs3DN, "KnotVector");
  if (knotVectorN.isNull()) {
    OCX_ERROR(
        "No KnotVector child node found in NURBS3D with curve id={} guid={}",
        id, guid);
    return {};
  }
  auto knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
  KnotMults kn = OCXHelper::ParseKnotVector(knotVectorS, numKnots);
  if (kn.IsNull) {
    OCX_ERROR("Failed to parse KnotVector in NURBS3D with curve id={} guid={}",
              id, guid);
    return {};
  }

  // Parse control Points
  LDOM_Element controlPtListN =
      OCXHelper::GetFirstChild(nurbs3DN, "ControlPtList");
  if (controlPtListN.isNull()) {
    OCX_ERROR(
        "No ControlPtList child node found in NURBS3D with curve id={} guid={}",
        id, guid);
    return {};
  }
  PolesWeightsCurve pw = OCXHelper::ParseControlPointsCurve(
      controlPtListN, numCtrlPoints, id, guid, ctx);
  if (pw.IsNull) {
    OCX_ERROR(
        "Failed to parse ControlPtList in NURBS3D with curve id={} guid={}", id,
        guid);
    return {};
  }

  // Create the curve
  Handle(Geom_BSplineCurve) curve =
      new Geom_BSplineCurve(pw.poles, pw.weights, kn.knots, kn.mults, degree,
                            Standard_False, isRational);

  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(curve);

  if (curve->IsClosed()) {
    return BRepBuilderAPI_MakeWire(edge);
  }
  return edge;
}

}  // namespace ocx
