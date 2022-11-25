// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-surface-reader.h"

#include <occutils/occutils-surface.h>
#include <occutils/occutils-wire.h>

#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BSplineSurface.hxx>
#include <ShapeFix_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>

#include "ocx/internal/ocx-curve-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-util.h"

namespace ocx {

TopoDS_Shape OCXSurfaceReader::ReadSurface(LDOM_Element const &surfaceN) const {
  char const *name = surfaceN.getAttribute("name").GetString();
  char const *id = surfaceN.getAttribute("id").GetString();
  char const *guid = surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString();

  std::string surfaceType = OCXHelper::GetLocalTagName(surfaceN);
  if (surfaceType == "SurfaceCollection") {
    return ReadSurfaceCollection(surfaceN, id, guid);
  } else if (surfaceType == "Cone3D") {
    return ReadCone3D(surfaceN, id, guid);
  } else if (surfaceType == "Cylinder3D") {
    return ReadCylinder3D(surfaceN, id, guid);
  } else if (surfaceType == "ExtrudedSurface") {
    return ReadExtrudedSurface(surfaceN, id, guid);
  } else if (surfaceType == "NURBSSurface") {
    return ReadNURBSSurface(surfaceN, id, guid);
  } else if (surfaceType == "Sphere3D") {
    return ReadSphere3D(surfaceN, id, guid);
  } else if (surfaceType == "Plane3D") {
    return ReadPlane3D(surfaceN, id, guid);
  } else {
    OCX_ERROR("Found unsupported surface type {} in surface {} id={} guid={}",
              surfaceType, name, id, guid);
    return {};
  }
}

TopoDS_Shape OCXSurfaceReader::ReadSurfaceCollection(
    LDOM_Element const &surfaceColN, std::string_view id,
    std::string_view guid) const {
  BRepBuilderAPI_Sewing shellMaker;

  LDOM_Node childN = surfaceColN.getFirstChild();
  auto face = TopoDS_Face();
  int numSurfaces = 0;
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)childN;
      char const *sId = surfaceN.getAttribute("id").GetString();

      std::string surfaceType = OCXHelper::GetLocalTagName(surfaceN);
      if (surfaceType == "Cone3D") {
        face = ReadCone3D(surfaceN, sId, guid);
      } else if (surfaceType == "Cylinder3D") {
        face = ReadCylinder3D(surfaceN, sId, guid);
      } else if (surfaceType == "ExtrudedSurface") {
        face = ReadExtrudedSurface(surfaceN, sId, guid);
      } else if (surfaceType == "NURBSSurface") {
        face = ReadNURBSSurface(surfaceN, sId, guid);
      } else if (surfaceType == "Sphere3D") {
        face = ReadSphere3D(surfaceN, sId, guid);
      } else if (surfaceType == "Plane3D") {
        face = ReadPlane3D(surfaceN, sId, guid);
      } else {
        OCX_ERROR(
            "Found unsupported surface type {} in SurfaceCollection with "
            "surface id={} guid={}",
            surfaceType, id, guid);
        childN = childN.getNextSibling();
        continue;
      }

      if (face.IsNull()) {
        OCX_ERROR(
            "Failed to read surface type {} in SurfaceCollection with surface "
            "id={} guid={}",
            surfaceType, id, guid);
        childN = childN.getNextSibling();
        continue;
      }
      shellMaker.Add(face);
      numSurfaces++;
    }
    childN = childN.getNextSibling();
  }

  if (numSurfaces == 0) {
    OCX_ERROR("No faces found in SurfaceCollection with surface id={} guid={}",
              id, guid);
    return {};
  }

  // Only one surface contained in SurfaceCollection, no need to sew
  if (numSurfaces == 1) {
    return face;
  }

  // https://dev.opencascade.org/content/brepbuilderapimakeshell
  shellMaker.Perform();
  TopoDS_Shape sewedShape = shellMaker.SewedShape();

  int numShells = 0;
  auto shape = TopoDS_Shell();
  TopExp_Explorer shellExplorer(sewedShape, TopAbs_SHELL);
  for (; shellExplorer.More(); shellExplorer.Next()) {
    shape = TopoDS::Shell(shellExplorer.Current());
    numShells++;
  }

  if (numShells != 1) {
    OCX_ERROR(
        "Expected exactly one shell to be composed from SurfaceCollection with "
        "surface id={} guid={}, but got {} shells, using last shell",
        id, guid, numShells);
  }

  return shape;
}

TopoDS_Face OCXSurfaceReader::ReadCone3D(LDOM_Element const &surfaceN,
                                         std::string_view id,
                                         std::string_view guid) const {
  OCX_ERROR("ReadCone3D not implemented yet");
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadCylinder3D(LDOM_Element const &surfaceN,
                                             std::string_view id,
                                             std::string_view guid) const {
  OCX_ERROR("ReadCylinder3D not implemented yet");
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadExtrudedSurface(LDOM_Element const &surfaceN,
                                                  std::string_view id,
                                                  std::string_view guid) const {
  OCX_ERROR("ReadExtrudedSurface not implemented yet");
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadNURBSSurface(LDOM_Element const &nurbsSrfN,
                                               std::string_view id,
                                               std::string_view guid) const {
  LDOM_Element faceBoundaryCurveN =
      OCXHelper::GetFirstChild(nurbsSrfN, "FaceBoundaryCurve");
  if (faceBoundaryCurveN.isNull()) {
    OCX_ERROR(
        "No FaceBoundaryCurve child node found in NURBSSurface with surface "
        "id={} "
        "guid={}",
        id, guid);
    return {};
  }
  OCXCurveReader curveReader(ctx);
  TopoDS_Wire outerContour = curveReader.ReadCurve(faceBoundaryCurveN);

  // Read U_NURBS properties
  LDOM_Element uPropsN =
      OCXHelper::GetFirstChild(nurbsSrfN, "U_NURBSproperties");
  if (uPropsN.isNull()) {
    OCX_ERROR(
        "No U_NURBSproperties child node found in NURBSSurface with surface "
        "id={} "
        "guid={}",
        id, guid);
    return {};
  }

  int uDegree{}, uNumCtrlPoints{}, uNumKnots{};
  uPropsN.getAttribute("degree").GetInteger(uDegree);
  uPropsN.getAttribute("numCtrlPts").GetInteger(uNumCtrlPoints);
  uPropsN.getAttribute("numKnots").GetInteger(uNumKnots);

  auto uForm = std::string(uPropsN.getAttribute("form").GetString());
  bool uIsRational = stob(uPropsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found U_NURBSproperties: degree={}, numCtrlPts={}, numKnots={}, "
      "form={}, isRational={}",
      uDegree, uNumCtrlPoints, uNumKnots, uForm.empty() ? "Unknown" : uForm,
      uIsRational);

  // Read V_NURBS properties
  LDOM_Element vPropsN =
      OCXHelper::GetFirstChild(nurbsSrfN, "V_NURBSproperties");
  if (vPropsN.isNull()) {
    OCX_ERROR(
        "No V_NURBSproperties child node found in NURBSSurface with surface "
        "id={} "
        "guid={}",
        id, guid);
    return {};
  }

  int vDegree{}, vNumCtrlPoints{}, vNumKnots{};
  vPropsN.getAttribute("degree").GetInteger(vDegree);
  vPropsN.getAttribute("numCtrlPts").GetInteger(vNumCtrlPoints);
  vPropsN.getAttribute("numKnots").GetInteger(vNumKnots);

  auto vForm = std::string(vPropsN.getAttribute("form").GetString());
  bool vIsRational = stob(vPropsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found V_NURBSproperties: degree={}, numCtrlPts={}, numKnots={}, "
      "form={}, isRational={}",
      vDegree, vNumCtrlPoints, vNumKnots, vForm.empty() ? "Unknown" : vForm,
      vIsRational);

  // Parse knotVectors
  LDOM_Element uKnotVectorN =
      OCXHelper::GetFirstChild(nurbsSrfN, "UknotVector");
  if (uKnotVectorN.isNull()) {
    OCX_ERROR(
        "No UknotVector child node found in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }
  auto uKnotVectorS =
      std::string(uKnotVectorN.getAttribute("value").GetString());
  KnotMults uKN = OCXHelper::ParseKnotVector(uKnotVectorS, uNumKnots);
  if (uKN.IsNull) {
    OCX_ERROR(
        "Failed to parse UknotVector in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }

  LDOM_Element vKnotVectorN =
      OCXHelper::GetFirstChild(nurbsSrfN, "VknotVector");
  if (uKnotVectorN.isNull()) {
    OCX_ERROR(
        "No VknotVector child node found in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }
  auto vKnotVectorS =
      std::string(vKnotVectorN.getAttribute("value").GetString());
  KnotMults vKN = OCXHelper::ParseKnotVector(vKnotVectorS, vNumKnots);
  if (vKN.IsNull) {
    OCX_ERROR(
        "Failed to parse VknotVector in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }

  // Parse control points
  LDOM_Element controlPtListN =
      OCXHelper::GetFirstChild(nurbsSrfN, "ControlPtList");
  if (controlPtListN.isNull()) {
    OCX_ERROR(
        "No ControlPtList child node found in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }
  PolesWeightsSurface pw = OCXHelper::ParseControlPointsSurface(
      controlPtListN, uNumCtrlPoints, vNumCtrlPoints, id, guid, ctx);
  if (pw.IsNull) {
    OCX_ERROR(
        "Failed to parse ControlPtList in NURBSSurface with surface id={} "
        "guid={}",
        id, guid);
    return {};
  }

  // Create the surface
  Handle(Geom_BSplineSurface) surface =
      new Geom_BSplineSurface(pw.poles, pw.weights, uKN.knots, vKN.knots,
                              uKN.mults, vKN.mults, uDegree, vDegree);
  if (surface.IsNull()) {
    OCX_ERROR(
        "Could not create Geom_BSplineSurface in NURBSSurface with surface "
        "id={} guid={}",
        id, guid);
    return {};
  }

  auto faceBuilder = BRepBuilderAPI_MakeFace(surface, outerContour);
  faceBuilder.Build();
  if (!faceBuilder.IsDone()) {
    OCX_ERROR(
        "Could not create restricted TopoDS_Face in NURBSSurface with surface "
        "id={} guid={}, exited with status {}",
        id, guid, faceBuilder.Error());
    return {};
  }

  // https://dev.opencascade.org/content/trimmed-nurbs-surface-creation
  ShapeFix_Face fix(faceBuilder.Face());
  fix.Perform();
  if (fix.Status(ShapeExtend_FAIL)) {
    OCX_ERROR(
        "Could not fix TopoDS_Face in NURBSSurface with surface id={} guid={}",
        id, guid);
    return {};
  }

  return fix.Face();
}

TopoDS_Face OCXSurfaceReader::ReadSphere3D(LDOM_Element const &surfaceN,
                                           std::string_view id,
                                           std::string_view guid) const {
  OCX_ERROR("ReadSphere3D not implemented yet");
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadPlane3D(LDOM_Element const &surfaceN,
                                          std::string_view id,
                                          std::string_view guid) const {
  LDOM_Element originN = OCXHelper::GetFirstChild(surfaceN, "Origin");
  if (originN.isNull()) {
    OCX_ERROR("No Origin child node found in Plane3D id={} guid={}", id, guid);
    return {};
  }
  gp_Pnt origin = OCXHelper::ReadPoint(originN, ctx);

  LDOM_Element normalN = OCXHelper::GetFirstChild(surfaceN, "Normal");
  if (normalN.isNull()) {
    OCX_ERROR(
        "No Normal child node found in Plane3D with surface id={} guid={}", id,
        guid);
    return {};
  }
  gp_Dir normal = OCXHelper::ReadDirection(normalN);

  // UDirection, FaceBoundaryCurve and LimitedBy are not mandatory in schema
  // TODO: read none mandatory elements from Plane3D

  auto plane = gp_Pln(origin, normal);
  TopoDS_Face planeFace = BRepBuilderAPI_MakeFace(plane);

  TopoDS_Wire outerContour{};
  LDOM_Element faceBoundaryCurveN =
      OCXHelper::GetFirstChild(surfaceN, "FaceBoundaryCurve");
  if (!faceBoundaryCurveN.isNull()) {
    OCXCurveReader curveReader(ctx);
    outerContour = curveReader.ReadCurve(faceBoundaryCurveN);
  } else {
    OCX_INFO(
        "No FaceBoundaryCurve child node found in Plane3D with surface id={} "
        "guid={}",
        id, guid);

    std::vector<gp_Pnt> pnts{};
    if (normal.IsParallel({1, 0, 0}, 1e-2)) {
      // YZ plane (frame)
      pnts.emplace_back(gp_Pnt(origin.X(), OCXContext::MinY, OCXContext::MinZ));
      pnts.emplace_back(gp_Pnt(origin.X(), OCXContext::MaxY, OCXContext::MinZ));
      pnts.emplace_back(gp_Pnt(origin.X(), OCXContext::MaxY, OCXContext::MaxZ));
      pnts.emplace_back(gp_Pnt(origin.X(), OCXContext::MinY, OCXContext::MaxZ));

    } else if (normal.IsParallel({0, 1, 0}, 1e-2)) {
      // XZ plane (longitudinal)
      pnts.emplace_back(gp_Pnt(OCXContext::MinX, origin.Y(), OCXContext::MinZ));
      pnts.emplace_back(gp_Pnt(OCXContext::MaxX, origin.Y(), OCXContext::MinZ));
      pnts.emplace_back(gp_Pnt(OCXContext::MaxX, origin.Y(), OCXContext::MaxZ));
      pnts.emplace_back(gp_Pnt(OCXContext::MinX, origin.Y(), OCXContext::MaxZ));

    } else if (normal.IsParallel({0, 0, 1}, 1e-2)) {
      // XY plane (deck)
      pnts.emplace_back(gp_Pnt(OCXContext::MinX, OCXContext::MinY, origin.Z()));
      pnts.emplace_back(gp_Pnt(OCXContext::MaxX, OCXContext::MinY, origin.Z()));
      pnts.emplace_back(gp_Pnt(OCXContext::MaxX, OCXContext::MaxY, origin.Z()));
      pnts.emplace_back(gp_Pnt(OCXContext::MinX, OCXContext::MaxY, origin.Z()));

    } else {
      // Set bounding box limit points
      gp_Pnt p0S(OCXContext::MinX, OCXContext::MinY, OCXContext::MinZ);
      gp_Pnt p1S(OCXContext::MinX, OCXContext::MaxY, OCXContext::MaxZ);
      gp_Pnt p2S(OCXContext::MaxX, OCXContext::MinY, OCXContext::MaxZ);
      gp_Pnt p3S(OCXContext::MaxX, OCXContext::MaxY, OCXContext::MinZ);

      // Define box line directions
      gp_Dir xDir(1, 0, 0);
      gp_Dir yDir(0, 1, 0);
      gp_Dir zDir(0, 0, 1);

      // Define box lines: indices are chosen in circular order
      //     p1S -------------------
      //     /|                   /|
      //   /  |                 /  |
      //  ------------------ p2S - p3S
      //  |  /                 |  /
      //  |/                   |/
      // p0s -------------------
      std::vector<gp_Lin> boxLines;
      boxLines.emplace_back(gp_Lin(p0S, xDir));
      boxLines.emplace_back(gp_Lin(p2S, xDir));
      boxLines.emplace_back(gp_Lin(p1S, xDir));
      boxLines.emplace_back(gp_Lin(p3S, xDir));

      boxLines.emplace_back(gp_Lin(p0S, yDir));
      boxLines.emplace_back(gp_Lin(p1S, yDir));
      boxLines.emplace_back(gp_Lin(p2S, yDir));
      boxLines.emplace_back(gp_Lin(p3S, yDir));

      boxLines.emplace_back(gp_Lin(p0S, zDir));
      boxLines.emplace_back(gp_Lin(p1S, zDir));
      boxLines.emplace_back(gp_Lin(p3S, zDir));
      boxLines.emplace_back(gp_Lin(p2S, zDir));

      GeomAdaptor_Surface surf = OCCUtils::Surface::FromFace(planeFace);

      for (const auto &boxLine : boxLines) {
        if (std::optional pnt = OCCUtils::Surface::Intersection(boxLine, surf);
            pnt.has_value()) {
          pnts.push_back(pnt.value());
        }
      }
    }
    outerContour = OCCUtils::Wire::FromPoints(pnts, true);
    if (!outerContour.Closed()) {
      OCX_ERROR(
          "Outer contour in ReadPlane3D is not closed. Skip building the "
          "Plane3D with surface id={} guid={}",
          id, guid);
      return {};
    }
  }

  auto faceBuilder = BRepBuilderAPI_MakeFace(planeFace, outerContour);
  faceBuilder.Build();
  if (!faceBuilder.IsDone()) {
    OCX_ERROR(
        "Could not create restricted TopoDS_Face in Plane3D with surface id={} "
        "guid={}, exited with status {}",
        id, guid, faceBuilder.Error());
    return {};
  }

  return faceBuilder.Face();
}

}  // namespace ocx
