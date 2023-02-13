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

#include "ocx/internal/ocx-surface.h"

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

#include "occutils/occutils-wire.h"
#include "ocx/internal/ocx-curve.h"
#include "ocx/internal/ocx-utils.h"
#include "ocx/ocx-helper.h"

namespace ocx::reader::shared::surface {

TopoDS_Shape ReadSurface(LDOM_Element const &surfaceN) {
  auto meta = ocx::helper::GetOCXMeta(surfaceN);

  std::string surfaceType = ocx::helper::GetLocalTagName(surfaceN);
  if (surfaceType == "SurfaceCollection") {
    return ReadSurfaceCollection(surfaceN);
  } else if (surfaceType == "Cone3D") {
    return ReadCone3D(surfaceN);
  } else if (surfaceType == "Cylinder3D") {
    return ReadCylinder3D(surfaceN);
  } else if (surfaceType == "ExtrudedSurface") {
    return ReadExtrudedSurface(surfaceN);
  } else if (surfaceType == "NURBSSurface") {
    return ReadNURBSSurface(surfaceN);
  } else if (surfaceType == "Sphere3D") {
    return ReadSphere3D(surfaceN);
  } else if (surfaceType == "Plane3D") {
    return ReadPlane3D(surfaceN);
  } else {
    OCX_ERROR("Found unsupported surface type {} in surface {} id={} guid={}",
              surfaceType, meta->name, meta->id, meta->guid)
    return {};
  }
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadSurfaceCollection(LDOM_Element const &surfaceColN) {
  auto meta = ocx::helper::GetOCXMeta(surfaceColN);

  BRepBuilderAPI_Sewing sew;

  LDOM_Node childN = surfaceColN.getFirstChild();
  auto face = TopoDS_Face();
  int numSurfaces = 0;
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)childN;

      std::string surfaceType = ocx::helper::GetLocalTagName(surfaceN);
      if (surfaceType == "Cone3D") {
        face = ReadCone3D(surfaceN);
      } else if (surfaceType == "Cylinder3D") {
        face = ReadCylinder3D(surfaceN);
      } else if (surfaceType == "ExtrudedSurface") {
        face = ReadExtrudedSurface(surfaceN);
      } else if (surfaceType == "NURBSSurface") {
        face = ReadNURBSSurface(surfaceN);
      } else if (surfaceType == "Sphere3D") {
        face = ReadSphere3D(surfaceN);
      } else if (surfaceType == "Plane3D") {
        face = ReadPlane3D(surfaceN);
      } else {
        OCX_ERROR(
            "Found unsupported surface type {} in SurfaceCollection with "
            "surface id={} guid={}",
            surfaceType, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }

      if (face.IsNull()) {
        OCX_ERROR(
            "Failed to read surface type {} in SurfaceCollection with surface "
            "id={} guid={}",
            surfaceType, meta->id, meta->guid)
        childN = childN.getNextSibling();
        continue;
      }
      sew.Add(face);
      numSurfaces++;
    }
    childN = childN.getNextSibling();
  }

  if (numSurfaces == 0) {
    OCX_ERROR("No faces found in SurfaceCollection with surface id={} guid={}",
              meta->id, meta->guid)
    return {};
  }

  // Only one surface contained in SurfaceCollection, no need to sew
  if (numSurfaces == 1) {
    return face;
  }

  sew.Perform();

  TopoDS_Shape sewedShape = sew.SewedShape();
  if (sewedShape.IsNull()) {
    OCX_ERROR(
        "Failed to sew faces in SurfaceCollection with surface id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  if (sewedShape.ShapeType() == TopAbs_COMPOUND) {
    OCX_WARN(
        "Sewed shape is of type TopAbs_COMPOUND, which is currently not fully"
        "supported")
    return sewedShape;
  }

  int numShells = 0;
  auto shell = TopoDS_Shell();
  for (TopExp_Explorer shellExplorer(sewedShape, TopAbs_SHELL);
       shellExplorer.More(); shellExplorer.Next()) {
    shell = TopoDS::Shell(shellExplorer.Current());
    numShells++;
  }

  if (numShells != 1) {
    OCX_ERROR(
        "Expected exactly one shell to be composed from SurfaceCollection with "
        "surface id={} guid={}, but got {} shells, using last shell",
        meta->id, meta->guid, numShells)
  }

  // Fix shells face orientation
  ShapeFix_Shell fix(shell);
  fix.Perform();
  if (fix.Status(ShapeExtend_FAIL)) {
    OCX_ERROR(
        "Failed to fix orientation of faces in shell of SurfaceCollection with "
        "surface id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  return fix.Shell();
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadCone3D(LDOM_Element const &surfaceN) {
  OCX_ERROR("ReadCone3D not implemented yet")
  return {};
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadCylinder3D(LDOM_Element const &surfaceN) {
  OCX_ERROR("ReadCylinder3D not implemented yet")
  return {};
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadExtrudedSurface(LDOM_Element const &surfaceN) {
  OCX_ERROR("ReadExtrudedSurface not implemented yet")
  return {};
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadNURBSSurface(LDOM_Element const &nurbsSrfN) {
  auto meta = ocx::helper::GetOCXMeta(nurbsSrfN);

  LDOM_Element faceBoundaryCurveN =
      ocx::helper::GetFirstChild(nurbsSrfN, "FaceBoundaryCurve");
  if (faceBoundaryCurveN.isNull()) {
    OCX_ERROR(
        "No FaceBoundaryCurve child node found in NURBSSurface with surface "
        "id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  TopoDS_Wire outerContour =
      ocx::reader::shared::curve::ReadCurve(faceBoundaryCurveN);
  if (outerContour.IsNull()) {
    OCX_ERROR(
        "Failed to read FaceBoundaryCurve child node in NURBSSurface with "
        "surface id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  // Read U_NURBS properties
  LDOM_Element uPropsN =
      ocx::helper::GetFirstChild(nurbsSrfN, "U_NURBSproperties");
  if (uPropsN.isNull()) {
    OCX_ERROR(
        "No U_NURBSproperties child node found in NURBSSurface with surface "
        "id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  int uDegree{}, uNumCtrlPoints{}, uNumKnots{};
  uPropsN.getAttribute("degree").GetInteger(uDegree);
  uPropsN.getAttribute("numCtrlPts").GetInteger(uNumCtrlPoints);
  uPropsN.getAttribute("numKnots").GetInteger(uNumKnots);

  auto uForm = std::string(uPropsN.getAttribute("form").GetString());
  bool uIsRational =
      ocx::utils::stob(uPropsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found U_NURBSproperties: degree={}, numCtrlPts={}, numKnots={}, "
      "form={}, isRational={}",
      uDegree, uNumCtrlPoints, uNumKnots, uForm.empty() ? "Unknown" : uForm,
      uIsRational)

  // Read V_NURBS properties
  LDOM_Element vPropsN =
      ocx::helper::GetFirstChild(nurbsSrfN, "V_NURBSproperties");
  if (vPropsN.isNull()) {
    OCX_ERROR(
        "No V_NURBSproperties child node found in NURBSSurface with surface "
        "id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  int vDegree{}, vNumCtrlPoints{}, vNumKnots{};
  vPropsN.getAttribute("degree").GetInteger(vDegree);
  vPropsN.getAttribute("numCtrlPts").GetInteger(vNumCtrlPoints);
  vPropsN.getAttribute("numKnots").GetInteger(vNumKnots);

  auto vForm = std::string(vPropsN.getAttribute("form").GetString());
  bool vIsRational =
      ocx::utils::stob(vPropsN.getAttribute("isRational").GetString());

  OCX_DEBUG(
      "Found V_NURBSproperties: degree={}, numCtrlPts={}, numKnots={}, "
      "form={}, isRational={}",
      vDegree, vNumCtrlPoints, vNumKnots, vForm.empty() ? "Unknown" : vForm,
      vIsRational)

  // Parse knotVectors
  LDOM_Element uKnotVectorN =
      ocx::helper::GetFirstChild(nurbsSrfN, "UknotVector");
  if (uKnotVectorN.isNull()) {
    OCX_ERROR(
        "No UknotVector child node found in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }
  auto uKnotVectorS =
      std::string(uKnotVectorN.getAttribute("value").GetString());
  ocx::helper::KnotMults uKN =
      ocx::helper::ParseKnotVector(uKnotVectorS, uNumKnots);
  if (uKN.IsNull) {
    OCX_ERROR(
        "Failed to parse UknotVector in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  LDOM_Element vKnotVectorN =
      ocx::helper::GetFirstChild(nurbsSrfN, "VknotVector");
  if (uKnotVectorN.isNull()) {
    OCX_ERROR(
        "No VknotVector child node found in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }
  auto vKnotVectorS =
      std::string(vKnotVectorN.getAttribute("value").GetString());
  ocx::helper::KnotMults vKN =
      ocx::helper::ParseKnotVector(vKnotVectorS, vNumKnots);
  if (vKN.IsNull) {
    OCX_ERROR(
        "Failed to parse VknotVector in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  // Parse control points
  LDOM_Element controlPtListN =
      ocx::helper::GetFirstChild(nurbsSrfN, "ControlPtList");
  if (controlPtListN.isNull()) {
    OCX_ERROR(
        "No ControlPtList child node found in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }
  ocx::helper::PolesWeightsSurface pw = ocx::helper::ParseControlPointsSurface(
      controlPtListN, uNumCtrlPoints, vNumCtrlPoints);
  if (pw.IsNull) {
    OCX_ERROR(
        "Failed to parse ControlPtList in NURBSSurface with surface id={} "
        "guid={}",
        meta->id, meta->guid)
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
        meta->id, meta->guid)
    return {};
  }

  auto faceBuilder = BRepBuilderAPI_MakeFace(surface, outerContour);
  faceBuilder.Build();
  if (!faceBuilder.IsDone()) {
    OCX_ERROR(
        "Could not create restricted TopoDS_Face in NURBSSurface with surface "
        "id={} guid={}, exited with status {}",
        meta->id, meta->guid, faceBuilder.Error())
    return {};
  }

  ShapeFix_Face fix(faceBuilder.Face());
  fix.Perform();
  if (fix.Status(ShapeExtend_FAIL)) {
    OCX_ERROR(
        "Could not fix TopoDS_Face in NURBSSurface with surface id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  return fix.Face();
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadSphere3D(LDOM_Element const &surfaceN) {
  OCX_ERROR("ReadSphere3D not implemented yet")
  return {};
}

//-----------------------------------------------------------------------------

TopoDS_Face ReadPlane3D(LDOM_Element const &surfaceN) {
  auto meta = ocx::helper::GetOCXMeta(surfaceN);

  LDOM_Element originN = ocx::helper::GetFirstChild(surfaceN, "Origin");
  if (originN.isNull()) {
    OCX_ERROR("No Origin child node found in Plane3D id={} guid={}", meta->id,
              meta->guid)
    return {};
  }

  LDOM_Element normalN = ocx::helper::GetFirstChild(surfaceN, "Normal");
  if (normalN.isNull()) {
    OCX_ERROR(
        "No Normal child node found in Plane3D with surface id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  gp_Pnt origin = ocx::helper::ReadPoint(originN);
  gp_Dir normal = ocx::helper::ReadDirection(normalN);

  // UDirection, FaceBoundaryCurve and LimitedBy are not mandatory in schema
  // TODO: read none mandatory elements from Plane3D

  auto plane = gp_Pln(origin, normal);
  TopoDS_Face planeFace = BRepBuilderAPI_MakeFace(plane);

  TopoDS_Wire outerContour{};
  LDOM_Element faceBoundaryCurveN =
      ocx::helper::GetFirstChild(surfaceN, "FaceBoundaryCurve");
  if (!faceBoundaryCurveN.isNull()) {
    outerContour = ocx::reader::shared::curve::ReadCurve(faceBoundaryCurveN);
  } else {
    OCX_DEBUG(
        "No FaceBoundaryCurve child node found in Plane3D with surface id={}. "
        "Going to restrict surface manually by global bounding box limits.",
        meta->id)

    std::vector<gp_Pnt> pnts{};
    if (normal.IsParallel({1, 0, 0}, 1e-2)) {
      // YZ plane (frame)
      pnts.emplace_back(origin.X(), OCXContext::MinY, OCXContext::MinZ);
      pnts.emplace_back(origin.X(), OCXContext::MaxY, OCXContext::MinZ);
      pnts.emplace_back(origin.X(), OCXContext::MaxY, OCXContext::MaxZ);
      pnts.emplace_back(origin.X(), OCXContext::MinY, OCXContext::MaxZ);

    } else if (normal.IsParallel({0, 1, 0}, 1e-2)) {
      // XZ plane (longitudinal)
      pnts.emplace_back(OCXContext::MinX, origin.Y(), OCXContext::MinZ);
      pnts.emplace_back(OCXContext::MaxX, origin.Y(), OCXContext::MinZ);
      pnts.emplace_back(OCXContext::MaxX, origin.Y(), OCXContext::MaxZ);
      pnts.emplace_back(OCXContext::MinX, origin.Y(), OCXContext::MaxZ);

    } else if (normal.IsParallel({0, 0, 1}, 1e-2)) {
      // XY plane (deck)
      pnts.emplace_back(OCXContext::MinX, OCXContext::MinY, origin.Z());
      pnts.emplace_back(OCXContext::MaxX, OCXContext::MinY, origin.Z());
      pnts.emplace_back(OCXContext::MaxX, OCXContext::MaxY, origin.Z());
      pnts.emplace_back(OCXContext::MinX, OCXContext::MaxY, origin.Z());

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
      // p0S -------------------
      std::vector<gp_Lin> boxLines;
      boxLines.emplace_back(p0S, xDir);
      boxLines.emplace_back(p2S, xDir);
      boxLines.emplace_back(p1S, xDir);
      boxLines.emplace_back(p3S, xDir);

      boxLines.emplace_back(p0S, yDir);
      boxLines.emplace_back(p1S, yDir);
      boxLines.emplace_back(p2S, yDir);
      boxLines.emplace_back(p3S, yDir);

      boxLines.emplace_back(p0S, zDir);
      boxLines.emplace_back(p1S, zDir);
      boxLines.emplace_back(p3S, zDir);
      boxLines.emplace_back(p2S, zDir);

      GeomAdaptor_Surface surf = OCCUtils::Surface::FromFace(planeFace);

      for (const auto &boxLine : boxLines) {
        if (std::optional pnt =
                OCCUtils::Surface::Intersection(boxLine, surf)) {
          pnts.push_back(pnt.value());
        }
      }
    }

    outerContour = OCCUtils::Wire::FromPoints(pnts, true);
    if (!outerContour.Closed()) {
      OCX_ERROR(
          "Outer contour in ReadPlane3D is not closed. Skip building the "
          "Plane3D with surface id={} guid={}",
          meta->id, meta->guid)
      return {};
    }
  }

  auto faceBuilder = BRepBuilderAPI_MakeFace(planeFace, outerContour);
  faceBuilder.Build();
  if (!faceBuilder.IsDone()) {
    OCX_ERROR(
        "Could not create restricted TopoDS_Face in Plane3D with surface id={} "
        "guid={}, exited with status {}",
        meta->id, meta->guid, faceBuilder.Error())
    return {};
  }

  return faceBuilder.Face();
}

}  // namespace

}  // namespace ocx::reader::shared::surface
