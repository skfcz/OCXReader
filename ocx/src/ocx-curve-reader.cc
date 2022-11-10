//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

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

OCXCurveReader::OCXCurveReader(std::shared_ptr<OCXContext> ctx)
    : ctx(std::move(ctx)) {}

TopoDS_Wire OCXCurveReader::ReadCurve(LDOM_Element const &curveRootN) {
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
        std::cerr << "found unknown curve type "
                  << curveN.getTagName().GetString() << std::endl;
        continue;
      }

      if (!edge.IsNull()) {
        switch (edge.ShapeType()) {
          case TopAbs_WIRE:
            // Return immediately if the shape is a wire
            return TopoDS::Wire(edge);
          case TopAbs_EDGE:
            // Add the edge to the wireBuilder
            wireBuilder.Add(TopoDS::Edge(edge));
            break;
          default:
            std::cerr << "found unknown shape type " << edge.ShapeType()
                      << std::endl;
            break;
        }
      }
    }
    childN = childN.getNextSibling();
  }
  return wireBuilder.Wire();
}

TopoDS_Shape OCXCurveReader::ReadCompositeCurve3D(LDOM_Element const &cCurveN) {
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
        edge = ReadLine3D(curveN);
      } else if (curveType == "CircumArc3D") {
        edge = ReadCircumArc3D(curveN);
      } else if (curveType == "NURBS3D") {
        edge = ReadNURBS3D(curveN);
      } else if (curveType == "PolyLine3D") {
        edge = ReadPolyLine3D(curveN);
      } else {
        std::cerr << "found unknown curve type in CompositeCurve3D "
                  << curveType << std::endl;
        continue;
      }

      if (!edge.IsNull()) {
        switch (edge.ShapeType()) {
          case TopAbs_WIRE:
            // Return immediately if the shape is a wire
            return TopoDS::Wire(edge);
          case TopAbs_EDGE:
            // Add the edge to the wireBuilder
            wireBuilder.Add(TopoDS::Edge(edge));
            break;
          default:
            std::cerr << "found unknown shape type in CompositeCurve3D "
                      << edge.ShapeType() << std::endl;
            break;
        }
      }
    }
    childN = childN.getNextSibling();
  }

  return wireBuilder.Wire();
}

TopoDS_Wire OCXCurveReader::ReadEllipse3D(LDOM_Element &ellipseN) {
  std::string id = std::string(ellipseN.getAttribute("id").GetString());
  std::cout << "ReadEllipse3D " << id << std::endl;

  LDOM_Element centerN = OCXHelper::GetFirstChild(ellipseN, "Center");
  if (centerN.isNull()) {
    std::cout << "could not find Ellipse3D/Center in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element majorDiaN = OCXHelper::GetFirstChild(ellipseN, "MajorDiameter");
  if (majorDiaN.isNull()) {
    std::cout << "could not find Ellipse3D/MajorDiameter in curve id='" << id
              << "'" << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element minorDiaN = OCXHelper::GetFirstChild(ellipseN, "MinorDiameter");
  if (majorDiaN.isNull()) {
    std::cout << "could not find Ellipse3D/MinorDiameter in curve id='" << id
              << "'" << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element majorAxisN = OCXHelper::GetFirstChild(ellipseN, "MajorAxis");
  if (majorAxisN.isNull()) {
    std::cout << "could not find Ellipse3D/MajorAxis in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element minorAxisN = OCXHelper::GetFirstChild(ellipseN, "MinorAxis");
  if (minorAxisN.isNull()) {
    std::cout << "could not find Ellipse3D/MinorAxis in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }

  gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
  double dMajor = OCXHelper::ReadDimension(majorDiaN, ctx);
  double dMinor = OCXHelper::ReadDimension(minorDiaN, ctx);
  gp_Dir majorAxis = OCXHelper::ReadDirection(majorAxisN);
  gp_Dir minorAxis = OCXHelper::ReadDirection(minorAxisN);
  gp_Dir normal = majorAxis.Crossed(minorAxis);

  gp_Ax2 cosys = gp_Ax2(center, normal, majorAxis);
  cosys.SetDirection(majorAxis);

  Handle(Geom_Ellipse) ellipse =
      new Geom_Ellipse(cosys, dMajor / 2.0, dMinor / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(ellipse);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire OCXCurveReader::ReadCircumCircle3D(LDOM_Element &circleN) {
  std::string id = std::string(circleN.getAttribute("id").GetString());
  std::cout << "ReadCircumCircle3D " << id << std::endl;

  LDOM_Element positionsN = OCXHelper::GetFirstChild(circleN, "Positions");
  if (positionsN.isNull()) {
    std::cout << "could not find CircumCircle/Positions with curve id='" << id
              << "'" << std::endl;
    return {};
  }

  LDOM_Node childN = positionsN.getFirstChild();

  gp_Pnt p0;
  gp_Pnt p1;
  gp_Pnt p2;

  int pointCnt = 0;

  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element pointN = (LDOM_Element &)childN;

      if ("Point3D" == OCXHelper::GetLocalTagName(pointN)) {
        if (pointCnt == 0) {
          p0 = OCXHelper::ReadPoint(pointN, ctx);
        } else if (pointCnt == 1) {
          p1 = OCXHelper::ReadPoint(pointN, ctx);
        } else if (pointCnt == 2) {
          p2 = OCXHelper::ReadPoint(pointN, ctx);
        } else {
          std::cout << "found more that 3 points in  CircumCircle/Positions "
                       "with curve id='"
                    << id << "'" << std::endl;
        }
        pointCnt++;
      } else {
        std::cout << "found element " << pointN.getTagName().GetString()
                  << " in  CircumCircle/Positions with curve id='" << id << "'"
                  << std::endl;
      }
    }
    childN = childN.getNextSibling();
  }

  Handle(Geom_Circle) circle = GC_MakeCircle(p0, p1, p2);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire OCXCurveReader::ReadCircle3D(LDOM_Element &circleN) {
  std::string id = std::string(circleN.getAttribute("id").GetString());
  std::cout << "ReadCircle3D " << id << std::endl;

  LDOM_Element centerN = OCXHelper::GetFirstChild(circleN, "Center");
  if (centerN.isNull()) {
    std::cout << "could not find Circle3D/Center in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element diaN = OCXHelper::GetFirstChild(circleN, "Diameter");
  if (diaN.isNull()) {
    std::cout << "could not find Circle3D/Diameter in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }
  LDOM_Element normalN = OCXHelper::GetFirstChild(circleN, "Normal");
  if (normalN.isNull()) {
    std::cout << "could not find Circle3D/Normal in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Wire();
  }

  gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
  double diameter = OCXHelper::ReadDimension(diaN, ctx);
  gp_Dir normal = OCXHelper::ReadDirection(normalN);

  gp_Ax2 cosys = gp_Ax2(center, normal);

  Handle(Geom_Circle) circle = new Geom_Circle(cosys, diameter / 2.0);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Edge OCXCurveReader::ReadCircumArc3D(LDOM_Element &circleN) {
  std::string id = std::string(circleN.getAttribute("id").GetString());
  std::cout << "ReadCircumArc3D " << id << std::endl;

  LDOM_Element startN = OCXHelper::GetFirstChild(circleN, "StartPoint");
  if (startN.isNull()) {
    std::cout << "could not find CircumArc3D/StartPoint in curve id='" << id
              << "'" << std::endl;
    return TopoDS_Edge();
  }
  LDOM_Element intN = OCXHelper::GetFirstChild(circleN, "IntermediatePoint");
  if (intN.isNull()) {
    std::cout << "could not find CircumArc3D/IntermediatePoint in curve id='"
              << id << "'" << std::endl;
    return TopoDS_Edge();
  }
  LDOM_Element endN = OCXHelper::GetFirstChild(circleN, "EndPoint");
  if (endN.isNull()) {
    std::cout << "could not find CircumArc3D/EndPoint in curve id='" << id
              << "'" << std::endl;
    return TopoDS_Edge();
  }

  gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
  gp_Pnt intermediate = OCXHelper::ReadPoint(intN, ctx);
  gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

  Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, end, intermediate);
  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Edge OCXCurveReader::ReadLine3D(LDOM_Element &lineN) {
  std::string id = std::string(lineN.getAttribute("id").GetString());
  std::cout << "ReadLine3D " << id << std::endl;

  LDOM_Element startN = OCXHelper::GetFirstChild(lineN, "StartPoint");
  if (startN.isNull()) {
    std::cout << "could not find Line3D/StartPoint in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Edge();
  }

  LDOM_Element endN = OCXHelper::GetFirstChild(lineN, "EndPoint");
  if (endN.isNull()) {
    std::cout << "could not find Line3D/EndPoint in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Edge();
  }

  gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
  gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

  Handle(Geom_TrimmedCurve) arc = GC_MakeSegment(start, end);
  return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Shape OCXCurveReader::ReadPolyLine3D(LDOM_Element &curveN) {
  std::cerr << "ReadPolyLine3D is not implemented yet" << std::endl;
  return {};
}

TopoDS_Shape OCXCurveReader::ReadNURBS3D(LDOM_Element const &nurbs3DN) const {
  auto id = std::string(nurbs3DN.getAttribute("id").GetString());
  std::cout << "ReadNURBS3D " << id << std::endl;

  LDOM_Element propsN = OCXHelper::GetFirstChild(nurbs3DN, "NURBSproperties");
  if (propsN.isNull()) {
    std::cout << "could not find NURBS3D/NURBSProperties in curve id='" << id
              << "'" << std::endl;
    return {};
  }
  int degree;
  propsN.getAttribute("degree").GetInteger(degree);
  int numCtrlPoints;
  propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPoints);
  int numKnots;
  propsN.getAttribute("numKnots").GetInteger(numKnots);
  auto form = std::string(propsN.getAttribute("form").GetString());
  bool isRational =
      stob(std::string(propsN.getAttribute("isRational").GetString()));

  // Parse knotVector
  LDOM_Element knotVectorN = OCXHelper::GetFirstChild(nurbs3DN, "KnotVector");
  if (knotVectorN.isNull()) {
    std::cout << "could not find NURBS3D/KnotVector in curve id='" << id << "'"
              << std::endl;
    return TopoDS_Edge();
  }
  auto knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
  KnotMults kn = OCXHelper::ParseKnotVector(knotVectorS, numKnots);
  if (kn.IsNull) {
    return {};
  }

  // Parse control Points
  LDOM_Element controlPtListN =
      OCXHelper::GetFirstChild(nurbs3DN, "ControlPtList");
  if (controlPtListN.isNull()) {
    std::cout << "could not find NURBS3D/ControlPtList in curve id='" << id
              << "'" << std::endl;
    return {};
  }
  PolesWeightsCurve pw = OCXHelper::ParseControlPointsCurve(
      controlPtListN, numCtrlPoints, id, ctx);
  if (pw.IsNull) {
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
