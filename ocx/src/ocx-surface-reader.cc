//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-surface-reader.h"

#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <memory>
#include <utility>

#include "ocx/internal/ocx-curve-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-util.h"

OCXSurfaceReader::OCXSurfaceReader(std::shared_ptr<OCXContext> ctx)
    : ctx(std::move(ctx)) {}

TopoDS_Shape OCXSurfaceReader::ReadSurface(LDOM_Element const &surfaceN) const {
  auto guid = std::string(surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString());
  auto id = std::string(surfaceN.getAttribute("id").GetString());

  std::string surfaceType = OCXHelper::GetLocalTagName(surfaceN);
  if (surfaceType == "SurfaceCollection") {
    return ReadSurfaceCollection(surfaceN, guid, id);
  } else if (surfaceType == "Cone3D") {
    return ReadCone3D(surfaceN, guid, id);
  } else if (surfaceType == "Cylinder3D") {
    return ReadCylinder3D(surfaceN, guid, id);
  } else if (surfaceType == "ReadExtrudedSurface") {
    return ReadExtrudedSurface(surfaceN, guid, id);
  } else if (surfaceType == "NURBSSurface") {
    return ReadNURBSurface(surfaceN, guid, id);
  } else if (surfaceType == "Sphere3D") {
    return ReadSphere3D(surfaceN, guid, id);
  } else if (surfaceType == "Plane3D") {
    return ReadPlane3D(surfaceN, guid, id);
  }
  std::cerr << "found unknown surface type "
            << surfaceN.getTagName().GetString() << ", guid (" << guid
            << "), id " << id << std::endl;
  return TopoDS_Shell();
}

TopoDS_Shape OCXSurfaceReader::ReadSurfaceCollection(
    LDOM_Element const &surfaceColN, std::string const &guid,
    std::string const &id) const {
  std::string colGuid =
      std::string(surfaceColN.getAttribute(ctx->OCXGUIDRef()).GetString());

  // https://github.com/DLR-SC/tigl/wiki/OpenCASCADE-Cheats#create-a-topods_shell-from-a-collection-of-topods_faces
  BRepBuilderAPI_Sewing shellMaker;

  LDOM_Node childN = surfaceColN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto face = TopoDS_Face();

      LDOM_Element surfaceN = (LDOM_Element &)childN;
      std::string surfaceType = OCXHelper::GetLocalTagName(surfaceN);
      if (surfaceType == "Cone3D") {
        face = ReadCone3D(surfaceN, guid, id);
      } else if (surfaceType == "Cylinder3D") {
        face = ReadCylinder3D(surfaceN, guid, id);
      } else if (surfaceType == "ReadExtrudedSurface") {
        face = ReadExtrudedSurface(surfaceN, guid, id);
      } else if (surfaceType == "NURBSSurface") {
        face = ReadNURBSurface(surfaceN, guid, id);
      } else if (surfaceType == "Sphere3D") {
        face = ReadSphere3D(surfaceN, guid, id);
      } else if (surfaceType == "Plane3D") {
        face = ReadPlane3D(surfaceN, guid, id);
      } else {
        std::cerr << "found unknown face type "
                  << surfaceN.getTagName().GetString()
                  << " in SurfaceCollection with guid '" << colGuid << "'"
                  << std::endl;
        continue;
      }

      if (face.IsNull()) {
        std::cerr << "failed to read from face "
                  << surfaceN.getTagName().GetString() << " (" << guid
                  << ") in SurfaceCollection with guid '" << colGuid << "'"
                  << std::endl;
      } else {
        shellMaker.Add(face);
      }
    }
    childN = childN.getNextSibling();
  }

  // https://dev.opencascade.org/content/brepbuilderapimakeshell
  shellMaker.Perform();

  TopoDS_Shape rawSewer = shellMaker.SewedShape();

  int numShells = 0;
  auto shape = TopoDS_Shape();
  TopExp_Explorer shellExplorer(rawSewer, TopAbs_SHELL);
  while (shellExplorer.More()) {
    // get current shape and convert to TopoDS_Shape
    shape = TopoDS::Shell(shellExplorer.Current());
    numShells++;

    shellExplorer.Next();
  }

  // Fallback to look for TopAbs_COMPOUND if no TopAbs_SHELL was found
  if (numShells == 0) {
    std::cout << "No TopAbs_SHELL found in shellMaker, try looking for "
                 "TopAbs_COMPOUND"
              << std::endl;
    TopExp_Explorer compExplorer(rawSewer, TopAbs_COMPOUND);
    while (compExplorer.More()) {
      // TODO: get current TopAbs_COMPOUND and convert to TopoDS_Shell


      // shape = TopoDS::Compound(compExplorer.Current());
      // numShells++;

      compExplorer.Next();
    }
  }

  if (numShells != 1) {
    std::cerr << "Expected one shell to be composed from SurfaceCollection "
              << " (" << colGuid << "), but got " << numShells
              << " shells, use only last" << std::endl;
  }

  return shape;
}

TopoDS_Face OCXSurfaceReader::ReadCone3D(LDOM_Element const &surfaceN,
                                         std::string const &guid,
                                         std::string const &id) const {
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadCylinder3D(LDOM_Element const &surfaceN,
                                             std::string const &guid,
                                             std::string const &id) const {
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadExtrudedSurface(LDOM_Element const &surfaceN,
                                                  std::string const &guid,
                                                  std::string const &id) const {
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadNURBSurface(LDOM_Element const &nurbsSrfN,
                                              std::string const &guid,
                                              std::string const &id) const {
  // Read FaceBoundaryCurve
  LDOM_Element faceBoundaryCurveN =
      OCXHelper::GetFirstChild(nurbsSrfN, "FaceBoundaryCurve");
  if (faceBoundaryCurveN.isNull()) {
    std::cout << "could not find  Plane3D/FaceBoundaryCurve in element with ("
              << guid << ")" << std::endl;
    return {};
  }
  auto *curveReader = new OCXCurveReader(ctx);
  TopoDS_Wire outerContour = curveReader->ReadCurve(faceBoundaryCurveN);

  // Read U_NURBS properties
  LDOM_Element uPropsN =
      OCXHelper::GetFirstChild(nurbsSrfN, "U_NURBSproperties");
  if (uPropsN.isNull()) {
    std::cerr << "could nit find failed NURBSSurface/U_NURBSproperties in "
                 "surface with id "
              << id << ", GUID" << guid << std::endl;
    return {};
  }
  int uDegree{}, uNumCtrlPoints{}, uNumKnots{};
  uPropsN.getAttribute("degree").GetInteger(uDegree);
  uPropsN.getAttribute("numCtrlPts").GetInteger(uNumCtrlPoints);
  uPropsN.getAttribute("numKnots").GetInteger(uNumKnots);
  auto uForm = std::string(uPropsN.getAttribute("form").GetString());
  bool uIsRational =
      stob(std::string(uPropsN.getAttribute("isRational").GetString()));
  std::cout << "U degree " << uDegree << ", #ctr " << uNumCtrlPoints
            << ", #knots " << uNumKnots << ", form '" << uForm
            << (uIsRational ? "', rational" : "', irrational") << std::endl;

  // Read V_NURBS properties
  LDOM_Element vPropsN =
      OCXHelper::GetFirstChild(nurbsSrfN, "V_NURBSproperties");
  if (vPropsN.isNull()) {
    std::cerr << "could nit find failed NURBSSurface/V_NURBSproperties in "
                 "surface with id "
              << id << ", GUID" << guid << std::endl;
    return {};
  }
  int vDegree{}, vNumCtrlPoints{}, vNumKnots{};
  vPropsN.getAttribute("degree").GetInteger(vDegree);
  vPropsN.getAttribute("numCtrlPts").GetInteger(vNumCtrlPoints);
  vPropsN.getAttribute("numKnots").GetInteger(vNumKnots);
  auto vForm = std::string(vPropsN.getAttribute("form").GetString());
  bool vIsRational =
      stob(std::string(vPropsN.getAttribute("isRational").GetString()));
  std::cout << "V degree " << vDegree << ", #ctr " << vNumCtrlPoints
            << ", #knots " << vNumKnots << ", form '" << vForm
            << (vIsRational ? "', rational" : "', irrational") << std::endl;

  // Parse knotVectors
  LDOM_Element uKnotVectorN =
      OCXHelper::GetFirstChild(nurbsSrfN, "UknotVector");
  if (uKnotVectorN.isNull()) {
    std::cout << "could not find NURBSSurface/UknotVector in surface id='" << id
              << "'" << std::endl;
    return {};
  }
  auto uKnotVectorS =
      std::string(uKnotVectorN.getAttribute("value").GetString());
  KnotMults uKN = OCXHelper::ParseKnotVector(uKnotVectorS, uNumKnots);
  if (uKN.IsNull) {
    return {};
  }

  LDOM_Element vKnotVectorN =
      OCXHelper::GetFirstChild(nurbsSrfN, "VknotVector");
  if (uKnotVectorN.isNull()) {
    std::cout << "could not find NURBSSurface/VknotVector in surface id='" << id
              << "'" << std::endl;
    return {};
  }
  auto vKnotVectorS =
      std::string(vKnotVectorN.getAttribute("value").GetString());
  KnotMults vKN = OCXHelper::ParseKnotVector(vKnotVectorS, vNumKnots);
  if (vKN.IsNull) {
    return {};
  }

  // Parse control points
  LDOM_Element controlPtListN =
      OCXHelper::GetFirstChild(nurbsSrfN, "ControlPtList");
  if (controlPtListN.isNull()) {
    std::cout << "could not find NURBSSurface/ControlPtList in surface id='"
              << id << "'" << std::endl;
    return {};
  }
  PolesWeightsSurface pw = OCXHelper::ParseControlPointsSurface(
      controlPtListN, uNumCtrlPoints, vNumCtrlPoints, id, ctx);
  if (pw.IsNull) {
    return {};
  }

  // Create the surface
  Handle(Geom_BSplineSurface) srf =
      new Geom_BSplineSurface(pw.poles, pw.weights, uKN.knots, vKN.knots,
                              uKN.mults, vKN.mults, uDegree, vDegree);
  auto faceBuilder = BRepBuilderAPI_MakeFace(srf, outerContour);
  faceBuilder.Build();
  TopoDS_Face restricted = faceBuilder.Face();
  if (restricted.IsNull()) {
    std::cerr
        << "failed to create a restricted surface from a NURBSSurface and "
           "FaceBoundaryCurve, guid "
        << guid << ", id " << id << ", error " << faceBuilder.Error()
        << std::endl;
    restricted = TopoDS_Face();
  }

  return restricted;
}

TopoDS_Face OCXSurfaceReader::ReadSphere3D(LDOM_Element const &surfaceN,
                                           std::string const &guid,
                                           std::string const &id) const {
  return {};
}

TopoDS_Face OCXSurfaceReader::ReadPlane3D(LDOM_Element const &surfaceN,
                                          std::string const &guid,
                                          std::string const &id) const {
  LDOM_Element originN = OCXHelper::GetFirstChild(surfaceN, "Origin");
  if (originN.isNull()) {
    std::cout
        << "could not read surface due to missing Plane3D/Origin element, id '"
        << guid << "'" << std::endl;
    return TopoDS_Face();
  }
  gp_Pnt origin = OCXHelper::ReadPoint(originN, ctx);

  LDOM_Element normalN = OCXHelper::GetFirstChild(surfaceN, "Normal");
  if (normalN.isNull()) {
    std::cout << "could not read surface due to missing Plane3D/Normal "
                 "element,  GUID is '"
              << guid << "'" << std::endl;
    return TopoDS_Face();
  }
  gp_Dir normal = OCXHelper::ReadDirection(normalN);

  // UDirection, FaceBoundaryCurve and LimitedBy are not mandatory in schema
  // TODO: read none mandatory elements from Plane3D

  gp_Pln plane = gp_Pln(origin, normal);
  TopoDS_Face planeFace = BRepBuilderAPI_MakeFace(plane);

  LDOM_Element faceBoundaryCurveN =
      OCXHelper::GetFirstChild(surfaceN, "FaceBoundaryCurve");
  if (faceBoundaryCurveN.isNull()) {
    std::cout << "could not find  Plane3D/FaceBoundaryCurve in element with ("
              << guid << ")" << std::endl;
    return planeFace;
  }

  auto *curveReader = new OCXCurveReader(ctx);
  TopoDS_Wire outerContour = curveReader->ReadCurve(faceBoundaryCurveN);

  auto faceBuilder = BRepBuilderAPI_MakeFace(planeFace, outerContour);
  faceBuilder.Build();
  TopoDS_Face restricted = faceBuilder.Face();
  if (restricted.IsNull()) {
    std::cerr << "failed to create a restricted surface from a Plane3D and "
                 "Plane3D/FaceBoundaryCurve, guid "
              << guid << ", id " << id << ", error " << faceBuilder.Error()
              << std::endl;
    restricted = TopoDS_Face();
  }

  return restricted;
}
