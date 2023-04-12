/***************************************************************************
 *   Created on: 31 May 2022                                               *
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

#include "ocx/ocx-helper.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <GeomAPI_IntSS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <memory>
#include <string>
#include <vector>

#include "occutils/occutils-face.h"
#include "occutils/occutils-shape-components.h"
#include "occutils/occutils-step-export.h"
#include "occutils/occutils-surface.h"
#include "ocx/internal/ocx-utils.h"
#include "ocx/ocx-context.h"

namespace ocx::helper {

std::unique_ptr<OCXMeta> GetOCXMeta(LDOM_Element const &element) {
  if (element.isNull()) return nullptr;

  char const *name = element.getAttribute("name").GetString();

  char const *id = element.getAttribute("id").GetString();

  char const *localRef = element.getAttribute("localRef").GetString();

  char const *guid = element.getAttribute("ocx:GUIDRef").GetString();

  std::string refType = element.getAttribute("ocx:refType").GetString();
  if (!refType.empty()) {
    if (std::size_t idx = refType.find(':'); idx != std::string::npos) {
      refType = refType.substr(idx + 1);
    }
  }

  return std::make_unique<OCXMeta>(name, id, localRef, guid, refType);
}

//-----------------------------------------------------------------------------

std::string GetLocalTagName(LDOM_Element const &elem, bool keepPrefix) {
  auto tagName = std::string(elem.getTagName().GetString());
  if (std::size_t idx = tagName.find(':');
      idx != std::string::npos && !keepPrefix) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

//-----------------------------------------------------------------------------

std::string GetLocalAttrName(LDOM_Node const &elem) {
  auto tagName = std::string(elem.getNodeName().GetString());
  if (std::size_t idx = tagName.find(':'); idx != std::string::npos) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

//-----------------------------------------------------------------------------

std::string GetAttrValue(LDOM_Element const &element, std::string const &name) {
  auto attributes = element.GetAttributesList();
  for (int i = 0; i < attributes.getLength(); i++) {
    LDOM_Node node = attributes.item(i);
    auto attrName = GetLocalAttrName(node);
    if (name == attrName) {
      return node.getNodeValue().GetString();
    }
  }
  return {};
}

//-----------------------------------------------------------------------------

std::string GetChildTagName(LDOM_Element const &parent, bool keepPrefix) {
  auto aChildNode = parent.getFirstChild();
  if (const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
      aNodeType == LDOM_Node::ELEMENT_NODE) {
    return GetLocalTagName((LDOM_Element &)aChildNode, keepPrefix);
  }
  return {};
}

//-----------------------------------------------------------------------------

LDOM_Element GetFirstChild(LDOM_Element const &parent,
                           std::string_view localName) {
  // Verify preconditions
  if (parent == nullptr || localName.length() == 0) {
    return {};
  }

  // Take the first child. If it doesn't match look for other ones in a loop
  LDOM_Node aChildNode = parent.getFirstChild();
  while (aChildNode != nullptr) {
    if (const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aNextElement = (LDOM_Element &)aChildNode;

      if (localName == GetLocalTagName(aNextElement)) {
        return aNextElement;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  return {};
}

//-----------------------------------------------------------------------------

void GetIntegerAttribute(LDOM_Element const &elem, std::string const &attrName,
                         Standard_Integer &value) {
  LDOMString stringValue = elem.getAttribute(attrName.c_str());
  if (stringValue.Type() == LDOMBasicString::StringType::LDOM_NULL) {
    return;
  }

  if (strlen(stringValue.GetString()) > 0) {
    value = Standard_Integer(std::stoi(stringValue.GetString()));
    return;
  }

  stringValue.GetInteger(value);
}

//-----------------------------------------------------------------------------

void GetDoubleAttribute(LDOM_Element const &elem, std::string const &attrName,
                        Standard_Real &value) {
  LDOMString stringValue = elem.getAttribute(attrName.c_str());
  if (stringValue.Type() == LDOMBasicString::StringType::LDOM_NULL) {
    return;
  }

  if (strlen(stringValue.GetString()) > 0) {
    value = Standard_Real(strtod(stringValue.GetString(), nullptr));
    return;
  }

  int i;
  stringValue.GetInteger(i);
  value = Standard_Real((double)i);
}

//-----------------------------------------------------------------------------

double ReadDimension(LDOM_Element const &valueN) {
  double value = 0;
  GetDoubleAttribute(valueN, "numericvalue", value);
  auto xUnit = std::string(valueN.getAttribute("unit").GetString());
  value *= OCXContext::GetInstance()->LoopupFactor(xUnit);
  return value;
}

//-----------------------------------------------------------------------------

gp_Pnt ReadPoint(LDOM_Element const &pointN) {
  auto xT = LDOMString("ocx:X");
  auto yT = LDOMString("ocx:Y");
  auto zT = LDOMString("ocx:Z");

  LDOM_Element xN = pointN.GetChildByTagName(xT);
  LDOM_Element yN = pointN.GetChildByTagName(yT);
  LDOM_Element zN = pointN.GetChildByTagName(zT);

  double x = ReadDimension(xN);
  double y = ReadDimension(yN);
  double z = ReadDimension(zN);

  return {x, y, z};
}

//-----------------------------------------------------------------------------

gp_Trsf ReadTransformation(LDOM_Element transfEle) {
  gp_Trsf localToGlobalTrsf;

  LDOM_Element originEle = GetFirstChild(transfEle, "Origin");
  if (!originEle.isNull()) {
    // Origin Point
    gp_Pnt point = ReadPoint(originEle);
    // Primary Axis Direction Vector
    LDOM_Element primaryAxisEle = GetFirstChild(transfEle, "PrimaryAxis");
    gp_Dir primAxis;
    if (!primaryAxisEle.isNull()) {
      primAxis = ReadDirection(primaryAxisEle);
    }

    // Secondary Axis Direction Vector
    LDOM_Element secondaryAxisEle = GetFirstChild(transfEle, "SecondaryAxis");
    gp_Dir secondaryAxis;
    if (!secondaryAxisEle.isNull()) {
      secondaryAxis = ReadDirection(secondaryAxisEle);
    }

    auto initialCoordinateSystem = gp_Ax3(gp::Origin(), gp::DZ());

    gp_Dir xAxis = primAxis;
    primAxis.Cross(secondaryAxis);
    auto targetCoordinateSystem = gp_Ax3(gp::Origin(), primAxis, xAxis);
    localToGlobalTrsf.SetDisplacement(initialCoordinateSystem,
                                      targetCoordinateSystem);

    gp_Trsf translation;
    translation.SetTranslation(gp::Origin(), point);
    localToGlobalTrsf = translation * localToGlobalTrsf;
  } else {
    OCX_ERROR("No origin found in transformation")
  }

  return localToGlobalTrsf;
}

//-----------------------------------------------------------------------------

gp_Dir ReadDirection(const LDOM_Element &dirN) {
  double x = 0;
  GetDoubleAttribute(dirN, "x", x);
  double y = 0;
  GetDoubleAttribute(dirN, "y", y);
  double z = 0;
  GetDoubleAttribute(dirN, "z", z);

  return {x, y, z};
}

//-----------------------------------------------------------------------------

KnotMults ParseKnotVector(std::string_view knotVectorS, int const &numKnots) {
  auto kn = KnotMults();

  std::vector<std::string> out;
  ocx::utils::Tokenize(knotVectorS, out);

  if (numKnots != out.size()) {
    OCX_ERROR(
        "Knot vector size mismatch. Expected {} knot values when parsing [{}], "
        "but got {}",
        numKnots, knotVectorS, out.size())
    kn.IsNull = true;
    return kn;
  }

  // Parse the knot values under consideration of multiplicity
  std::vector<double> knots0;
  std::vector<int> mults0;
  // Get the quantity of knots and their multiplicities values
  double lastKnot{};
  int mult{};
  for (int i = 0; i < out.size(); i++) {
    double knotValue = std::stod(out[i]);
    // First knot value
    if (i == 0) {
      lastKnot = knotValue;
      mult = 1;
      continue;
    }
    // Increase multiplicity if knot value is nearly the same as the last one
    if (abs(knotValue - lastKnot) < 1e-9) {
      mult++;
      // Don't forget to push, if it's the last knot value
      if (i == out.size() - 1) {
        knots0.push_back(knotValue);
        mults0.push_back(mult);
      }
    } else {
      knots0.push_back(lastKnot);
      mults0.push_back(mult);
      lastKnot = knotValue;
      mult = 1;
    }
  }

  // Resize the knot and multiplicity arrays to the needed size
  kn.knots.Resize(1, (int)knots0.size(), false);
  kn.mults.Resize(1, (int)mults0.size(), false);

  // Set the knot and multiplicity values
  for (int i = 0; i < knots0.size(); i++) {
    kn.knots.SetValue(i + 1, knots0[i]);
    kn.mults.SetValue(i + 1, mults0[i]);
  }

  return kn;
}

//-----------------------------------------------------------------------------

PolesWeightsCurve ParseControlPointsCurve(LDOM_Element const &controlPtListN,
                                          int const &numCtrlPoints) {
  auto polesWeights = PolesWeightsCurve(numCtrlPoints);

  LDOM_Node childN = controlPtListN.getFirstChild();
  if (childN.isNull()) {
    OCX_ERROR("No child node found in given ControlPointList element")
    polesWeights.IsNull = true;
    return polesWeights;
  }

  int i = 1;  // index starts at 1
  while (childN != nullptr) {
    // Check for valid node type
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType != LDOM_Node::ELEMENT_NODE) {
      OCX_ERROR(
          "Invalid node type found in ControlPointList element. Got {}, but"
          "expected {}",
          nodeType, LDOM_Node::ELEMENT_NODE)
      polesWeights.IsNull = true;
      return polesWeights;
    }

    // Parse the control point
    LDOM_Element controlPointN = (LDOM_Element &)childN;
    LDOM_Element pointN = GetFirstChild(controlPointN, "Point3D");
    if (pointN.isNull()) {
      OCX_ERROR("No Point3D child node found in given ControlPoint element")
      polesWeights.IsNull = true;
      return polesWeights;
    }

    // Read the point
    polesWeights.poles.SetValue(i, ReadPoint(pointN));

    // Read the weight
    double weight = 1.0;  // default weight
    GetDoubleAttribute(pointN, "weight", weight);
    polesWeights.weights.SetValue(i, weight);

    childN = controlPointN.getNextSibling();
    i++;
  }

  //  for ( int i = 1; i <= polesWeights.poles.Size();i++) {
  //    gp_Pnt point = polesWeights.poles.Value(i);
  //      OCX_INFO("pole #{} [{}, {}, {}]", i, point.X(), point.Y(), point.Z());
  //  }

  return polesWeights;
}

//-----------------------------------------------------------------------------

PolesWeightsSurface ParseControlPointsSurface(
    LDOM_Element const &controlPtListN, int const &uNumCtrlPoints,
    int const &vNumCtrlPoints) {
  auto polesWeights = PolesWeightsSurface(uNumCtrlPoints, vNumCtrlPoints);

  LDOM_Node childN = controlPtListN.getFirstChild();
  if (childN.isNull()) {
    OCX_ERROR("No child node found in given ControlPointList element")
    polesWeights.IsNull = true;
    return polesWeights;
  }

  for (int u = 1; u <= uNumCtrlPoints; u++) {
    for (int v = 1; v <= vNumCtrlPoints; v++) {
      // Check for valid node type
      const LDOM_Node::NodeType nodeType = childN.getNodeType();
      if (nodeType != LDOM_Node::ELEMENT_NODE) {
        OCX_ERROR(
            "Invalid node type found in ControlPointList element. Got {}, but"
            "expected {}",
            nodeType, LDOM_Node::ELEMENT_NODE)
        polesWeights.IsNull = true;
        return polesWeights;
      }

      // Parse the control point
      LDOM_Element controlPointN = (LDOM_Element &)childN;
      LDOM_Element pointN = GetFirstChild(controlPointN, "Point3D");
      if (pointN.isNull()) {
        OCX_ERROR("No Point3D child node found in given ControlPoint element")
        polesWeights.IsNull = true;
        return polesWeights;
      }

      // Read the point
      polesWeights.poles.SetValue(u, v, ReadPoint(pointN));

      // Read the weight
      double weight = 1.0;  // default weight
      GetDoubleAttribute(pointN, "weight", weight);
      polesWeights.weights.SetValue(u, v, weight);

      childN = controlPointN.getNextSibling();
    }
  }

  return polesWeights;
}

//-----------------------------------------------------------------------------

// TODO: Prototype, if solution is proven to work goes to -> OCCUtils::Surface
// TODO: Should support SewedShape shape type (face, a shell, a solid or a
// TODO: compound.)
TopoDS_Shape LimitShapeByWire(TopoDS_Shape const &shape,
                              TopoDS_Wire const &wire, std::string_view id,
                              std::string_view guid) {
  // Check if given TopoDS_Shape is one of TopoDS_Face or TopoDS_Shell
  if (!OCCUtils::Shape::IsFace(shape) && !OCCUtils::Shape::IsShell(shape)) {
    OCX_ERROR(
        "Given TopoDS_Shape is neither a TopoDS_Face or a TopoDS_Shell in "
        "Shape id={} guid={}",
        id, guid)
    return {};
  }

  // Handle TopoDS_Shape is a TopoDS_Face
  if (OCCUtils::Shape::IsFace(shape)) {
    try {
      GeomAdaptor_Surface surfaceAdapter =
          OCCUtils::Surface::FromFace(TopoDS::Face(shape));
      auto faceBuilder = BRepBuilderAPI_MakeFace(surfaceAdapter.Surface(), wire,
                                                 Standard_True);
      faceBuilder.Build();
      if (!faceBuilder.IsDone()) {
        OCX_ERROR(
            "Failed to create restricted Shape from given Surface and "
            "OuterContour in id={} guid={}",
            id, guid)
        return {};
      }
      return faceBuilder.Face();
    } catch (Standard_Failure const &e) {
      OCX_ERROR(
          "Failed to create restricted Shape from given Surface and "
          "OuterContour in id={} guid={}",
          id, guid)
      return {};
    }
  }

  if (false) {
    // TODO: Currently cutting curved surfaces with a wire is not supported
    // TODO: BRepBuilderAPI_MakeFace: Make a face from a Surface and a wire. If
    // TODO: the surface S is not plane, it must contain pcurves for all edges
    // in
    // TODO:  W, otherwise the wrong shape will be created.
    // TODO: Check better approach of wire imprinting on the surface
    // http://analysissitus.org/forum/index.php?threads/create-restricted-surface-from-topods_shell-and-topods_wire-plate-extraction.217/

    // Else handle TopoDS_Shell
    std::vector<TopoDS_Face> faces =
        OCCUtils::ShapeComponents::AllFacesWithin(shape);

    BRepBuilderAPI_Sewing shellMaker;
    for (TopoDS_Face const &face : faces) {
      GeomAdaptor_Surface surfaceAdapter =
          OCCUtils::Surface::FromFace(TopoDS::Face(face));
      auto faceBuilder = BRepBuilderAPI_MakeFace(surfaceAdapter.Surface(), wire,
                                                 Standard_True);
      faceBuilder.Build();
      if (!faceBuilder.IsDone()) {
        OCX_ERROR(
            "Failed to create restricted Shape from given Surface and "
            "OuterContour in id={} guid={}",
            id, guid);
        return {};
      }

      ShapeFix_Face fix(faceBuilder.Face());
      fix.Perform();
      if (fix.Status(ShapeExtend_FAIL)) {
        OCX_ERROR("Could not fix TopoDS_Face from read face in id={} guid={}",
                  id, guid);
        return {};
      }
      shellMaker.Add(fix.Face());
    }

    shellMaker.Perform();
    TopoDS_Shape sewedShape = shellMaker.SewedShape();

    int numShells = 0;
    auto shell = TopoDS_Shell();
    TopExp_Explorer shellExplorer(sewedShape, TopAbs_SHELL);
    for (; shellExplorer.More(); shellExplorer.Next()) {
      shell = TopoDS::Shell(shellExplorer.Current());
      numShells++;
    }

    if (numShells != 1) {
      OCX_ERROR(
          "Expected exactly one shell to be composed from LimitShapeByWire in "
          "id={} guid={}, but got {} shells, skip it. This is must likely due "
          "to "
          "missing implementation building a TopoDS_Face from non-planar "
          "surface "
          "and wire",
          id, guid, numShells);
      return {};
    }
    return shell;
  }

  return {};
}

//-----------------------------------------------------------------------------

// TODO: Prototype, if solution is proven to work goes to -> OCCUtils::Surface
std::optional<TopoDS_Edge> Intersection(const GeomAdaptor_Surface &S1,
                                        const GeomAdaptor_Surface &S2) {
  auto intersector =
      GeomAPI_IntSS(S1.Surface(), S2.Surface(), Precision::Confusion());
  if (!intersector
           .IsDone()) {  // Algorithm failure, returned as no intersection
    return std::nullopt;
  }
  if (intersector.NbLines() == 0 || intersector.NbLines() > 1) {
    return std::nullopt;
  }
  return BRepBuilderAPI_MakeEdge(intersector.Line(1)).Edge();
}

//-----------------------------------------------------------------------------

// TODO: Prototype, if solution is proven to work goes to -> OCCUtils::Curve
std::optional<TopoDS_Edge> CurveLimitByBoundingBox(
    const GeomAdaptor_Curve &curve, const Bnd_Box &box) {
  if (box.IsOut(curve.Line())) {
    return std::nullopt;
  }
  gp_Pnt max = box.CornerMin();
  gp_Pnt min = box.CornerMax();

  // Get bounding box planes as faces
  std::vector<TopoDS_Face> faces{};
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(min.X(), min.Y(), min.Z()), gp_Pnt(max.X(), min.Y(), min.Z()),
       gp_Pnt(max.X(), max.Y(), min.Z())}));  // XYBottom
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(min.X(), min.Y(), max.Z()), gp_Pnt(max.X(), min.Y(), max.Z()),
       gp_Pnt(max.X(), max.Y(), max.Z())}));  // XYTop
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(min.X(), min.Y(), min.Z()), gp_Pnt(min.X(), max.Y(), min.Z()),
       gp_Pnt(min.X(), max.Y(), max.Z())}));  // XZLeft
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(max.X(), min.Y(), min.Z()), gp_Pnt(max.X(), max.Y(), min.Z()),
       gp_Pnt(max.X(), max.Y(), max.Z())}));  // XZRight
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(min.X(), min.Y(), min.Z()), gp_Pnt(min.X(), min.Y(), max.Z()),
       gp_Pnt(max.X(), min.Y(), max.Z())}));  // YZFront
  faces.emplace_back(OCCUtils::Face::FromPoints(
      {gp_Pnt(min.X(), max.Y(), min.Z()), gp_Pnt(min.X(), max.Y(), max.Z()),
       gp_Pnt(max.X(), max.Y(), max.Z())}));  // YZBack

  // Intersect faces with curve
  std::vector<gp_Pnt> intersectionPoints{};
  for (const auto &face : faces) {
    GeomAdaptor_Surface surfaceAdapter =
        OCCUtils::Surface::FromFace(TopoDS::Face(face));

    if (std::optional pnt = OCCUtils::Surface::Intersection(
            curve.Line(), surfaceAdapter.Surface())) {
      intersectionPoints.push_back(pnt.value());
    }
  }

  if (intersectionPoints.size() != 2) {
    return std::nullopt;
  }

  // Limit the curve to the bounding box
  return BRepBuilderAPI_MakeEdge(curve.Line(), intersectionPoints[0],
                                 intersectionPoints[1])
      .Edge();
}

}  // namespace ocx::helper
