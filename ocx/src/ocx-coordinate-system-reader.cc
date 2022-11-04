//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-coordinate-system-reader.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Builder.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pln.hxx>
#include <list>
#include <memory>
#include <utility>

#include "ocx/internal/ocx-helper.h"

OCXCoordinateSystemReader::OCXCoordinateSystemReader(
    std::shared_ptr<OCXContext> ctx)
    : ctx(std::move(ctx)) {}

TopoDS_Shape OCXCoordinateSystemReader::ReadCoordinateSystem(
    LDOM_Element &vesselN) {
  std::string tag = std::string(vesselN.getTagName().GetString());
  std::size_t index = tag.find("Vessel");
  if (index == std::string::npos) {
    std::cout << "expected a Vessel element, got " << tag << std::endl;
    return TopoDS_Shape();
  }

  LDOM_Element cosysN = OCXHelper::GetFirstChild(vesselN, "CoordinateSystem");
  if (cosysN.isNull()) {
    std::cout << "could not find CoordinateSystem child node" << std::endl;
    return TopoDS_Shape();
  }

  // TODO: do we need to read the LocalCartesian ?
  // TODO: do we need to read the VesselGrid ?

  std::list<TopoDS_Shape> shapes;

  LDOM_Element frTblsN = OCXHelper::GetFirstChild(cosysN, "FrameTables");
  if (frTblsN.isNull()) {
    std::cout << "could not find CoordinateSystem/FrameTables child node"
              << std::endl;
    return TopoDS_Shape();
  }

  LDOM_Element xRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "XRefPlanes");
  if (xRefPlanesN.isNull()) {
    std::cout
        << "could not find CoordinateSystem/FrameTables/XRefPlanes child node"
        << std::endl;
  } else {
    TopoDS_Shape comp = ReadRefPlanes(xRefPlanesN);
    shapes.push_back(comp);
  }

  LDOM_Element yRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "YRefPlanes");
  if (yRefPlanesN.isNull()) {
    std::cout
        << "could not find CoordinateSystem/FrameTables/YRefPlanes child node"
        << std::endl;
  } else {
    TopoDS_Shape comp = ReadRefPlanes(yRefPlanesN);
    shapes.push_back(comp);
  }

  LDOM_Element zRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "ZRefPlanes");
  if (zRefPlanesN.isNull()) {
    std::cout
        << "could not find CoordinateSystem/FrameTables/ZRefPlanes child node"
        << std::endl;
  } else {
    TopoDS_Shape comp = ReadRefPlanes(zRefPlanesN);
    shapes.push_back(comp);
  }

  TopoDS_Compound refPlanesAssy;
  BRep_Builder bbuilder;
  bbuilder.MakeCompound(refPlanesAssy);
  for (TopoDS_Shape shape : shapes) {
    bbuilder.Add(refPlanesAssy, shape);
  }

  TDF_Label label = ctx->OCAFShapeTool()->AddShape(
      refPlanesAssy);  //, std::false_type, std::false_type);
  TDataStd_Name::Set(label, "Reference Planes");

  return refPlanesAssy;
}

TopoDS_Shape OCXCoordinateSystemReader::ReadRefPlanes(
    LDOM_Element &refPlanesN) {
  std::cout << "ReadRefPlanes from " << refPlanesN.getTagName().GetString()
            << std::endl;

  std::list<TopoDS_Shape> shapes;

  std::string containerName = "";

  Quantity_Color color = Quantity_Color(1, 0, 0, Quantity_TOC_RGB);
  // too lazy to define enumeration
  int axis = -1;
  if (strstr(refPlanesN.getTagName().GetString(), "XRefPlanes")) {
    axis = 0;
    containerName = "XRefPlanes";
    // Material Green 400
    color =
        Quantity_Color(102 / 256.0, 187 / 256.0, 106 / 256, Quantity_TOC_RGB);
  } else if (strstr(refPlanesN.getTagName().GetString(), "YRefPlanes")) {
    axis = 1;
    containerName = "YRefPlanes";
    // Material Lime 400
    color =
        Quantity_Color(212 / 256.0, 225 / 256.0, 87 / 256, Quantity_TOC_RGB);
  } else if (strstr(refPlanesN.getTagName().GetString(), "ZRefPlanes")) {
    axis = 2;
    containerName = "ZRefPlanes";
    // Material Amber 400
    color =
        Quantity_Color(255 / 256.0, 202 / 256.0, 40 / 256, Quantity_TOC_RGB);
  } else {
    std::cout << "expected one element from XRefPlanes, YRefPlanes, or "
                 "ZRefPlanes, got '"
              << refPlanesN.getTagName().GetString() << "'" << std::endl;
    return TopoDS_Shape();
  }

  int cntPlanes = 0;
  LDOM_Node aChildN = refPlanesN.getFirstChild();
  while (!aChildN.isNull()) {
    const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();

    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) {
      break;
    }
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element refPlaneN = (LDOM_Element &)aChildN;

      if ("RefPlane" == OCXHelper::GetLocalTagName(refPlaneN)) {
        std::string guid =
            std::string(refPlaneN.getAttribute(ctx->OCXGUIDRef()).GetString());
        std::string name =
            std::string(refPlaneN.getAttribute("name").GetString());

        // std::cout << "ref plane " << name << ", " << guid << std::endl;

        LDOM_Element refLocN =
            OCXHelper::GetFirstChild(refPlaneN, "ReferenceLocation");
        if (refLocN.isNull()) {
          std::cout << "could not RefPlane/ReferenceLocation where GUIDRef is '"
                    << guid << "', skip it" << std::endl;
        } else {
          double location;
          OCXHelper::GetDoubleAttribute(refLocN, "numericvalue", location);
          std::string xUnit =
              std::string(refLocN.getAttribute("unit").GetString());
          location *= ctx->LoopupFactor(xUnit);

          // std::cout << "    location  " << location <<  std::endl;

          // Now create an OCC Plane
          double width = 50;
          double minX = -10;
          double maxX = 190;
          double minZ = -5;
          double maxZ = 45;

          gp_Pnt org;
          gp_Dir direction;
          gp_Pnt pnt0, pnt1, pnt2, pnt3;

          if (axis == 0) {
            // frames
            org = gp_Pnt(location, 0, 0);
            direction = gp_Dir(1, 0, 0);
            pnt0 = gp_Pnt(location, 1.10 * width / 2.0, minZ);
            pnt1 = gp_Pnt(location, -1.10 * width / 2.0, minZ);
            pnt2 = gp_Pnt(location, -1.10 * width / 2.0, maxZ);
            pnt3 = gp_Pnt(location, 1.10 * width / 2.0, maxZ);

          } else if (axis == 1) {
            // longitudinals
            org = gp_Pnt(0, location, 0);
            direction = gp_Dir(0, 1, 0);

            pnt0 = gp_Pnt(minX, location, minZ);
            pnt1 = gp_Pnt(maxX, location, minZ);
            pnt2 = gp_Pnt(maxX, location, maxZ);
            pnt3 = gp_Pnt(minX, location, maxZ);

          } else if (axis == 2) {
            // verticals
            org = gp_Pnt(0, 0, location);
            direction = gp_Dir(0, 0, 1);

            pnt0 = gp_Pnt(minX, 1.05 * width / 2.0, location);
            pnt1 = gp_Pnt(minX, -1.05 * width / 2.0, location);
            pnt2 = gp_Pnt(maxX, -1.05 * width / 2.0, location);
            pnt3 = gp_Pnt(maxX, 1.05 * width / 2.0, location);
          }

          auto unlimitedSurface = gp_Pln(org, direction);

          // ... and a wire around it
          Handle(Geom_TrimmedCurve) seg01 = GC_MakeSegment(pnt0, pnt1);
          Handle(Geom_TrimmedCurve) seg12 = GC_MakeSegment(pnt1, pnt2);
          Handle(Geom_TrimmedCurve) seg23 = GC_MakeSegment(pnt2, pnt3);
          Handle(Geom_TrimmedCurve) seg30 = GC_MakeSegment(pnt3, pnt0);

          TopoDS_Edge edge0 = BRepBuilderAPI_MakeEdge(seg01);
          TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(seg12);
          TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(seg23);
          TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(seg30);

          BRepBuilderAPI_MakeWire makeWire;
          makeWire.Add(edge0);
          makeWire.Add(edge1);
          makeWire.Add(edge2);
          makeWire.Add(edge3);
          makeWire.Build();

          TopoDS_Wire wire = makeWire.Wire();
          TopoDS_Face surface = BRepBuilderAPI_MakeFace(unlimitedSurface, wire);
          ctx->RegisterSurface(guid, surface);

          TDF_Label surfL = ctx->OCAFShapeTool()->AddShape(surface, false);
          TDataStd_Name::Set(surfL, name.c_str());
          ctx->OCAFColorTool()->SetColor(surfL, color, XCAFDoc_ColorSurf);

          shapes.push_back(surface);

          cntPlanes++;
        }

      } else {
        std::cerr << "expected RefPlane element, got  "
                  << refPlaneN.getTagName().GetString() << std::endl;
      }
    }

    aChildN = aChildN.getNextSibling();

  }  // end iterate over reference planes

  TopoDS_Compound refPlanesXYZAssy;
  BRep_Builder refPlanesBuilder;
  refPlanesBuilder.MakeCompound(refPlanesXYZAssy);
  for (const TopoDS_Shape &shape : shapes) {
    refPlanesBuilder.Add(refPlanesXYZAssy, shape);
  }

  TDF_Label refSrfLabel =
      ctx->OCAFShapeTool()->AddShape(refPlanesXYZAssy, true);
  TDataStd_Name::Set(refSrfLabel, containerName.c_str());

  std::cout << "    registered #" << cntPlanes << " planes found in "
            << refPlanesN.getTagName().GetString() << std::endl;

  return refPlanesXYZAssy;
}
