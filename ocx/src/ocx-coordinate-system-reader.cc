// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

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
#include <TopoDS_Compound.hxx>
#include <gp_Pln.hxx>
#include <list>

#include "ocx/internal/ocx-helper.h"

namespace ocx {

TopoDS_Shape OCXCoordinateSystemReader::ReadCoordinateSystem(
    LDOM_Element const &vesselN) {
  OCX_INFO("Start reading coordinate system...");

  LDOM_Element cosysN = OCXHelper::GetFirstChild(vesselN, "CoordinateSystem");
  if (cosysN.isNull()) {
    OCX_ERROR("No CoordinateSystem child node found");
    return {};
  }

  // TODO: do we need to read the LocalCartesian ?
  // TODO: do we need to read the VesselGrid ?

  LDOM_Element frTblsN = OCXHelper::GetFirstChild(cosysN, "FrameTables");
  if (frTblsN.isNull()) {
    OCX_ERROR("No FrameTables child node found in CoordinateSystem");
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Element xRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "XRefPlanes");
  if (xRefPlanesN.isNull()) {
    OCX_ERROR("No XRefPlanes child node found in CoordinateSystem/FrameTables");
  } else {
    // Material Green 400
    auto color =
        Quantity_Color(102 / 256.0, 187 / 256.0, 106 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(xRefPlanesN, color);
    shapes.push_back(comp);
  }

  LDOM_Element yRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "YRefPlanes");
  if (yRefPlanesN.isNull()) {
    OCX_ERROR("No YRefPlanes child node found in CoordinateSystem/FrameTables");
  } else {
    // Material Lime 400
    auto color =
        Quantity_Color(212 / 256.0, 225 / 256.0, 87 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(yRefPlanesN, color);
    shapes.push_back(comp);
  }

  LDOM_Element zRefPlanesN = OCXHelper::GetFirstChild(frTblsN, "ZRefPlanes");
  if (zRefPlanesN.isNull()) {
    OCX_ERROR("No ZRefPlanes child node found in CoordinateSystem/FrameTables");
  } else {
    // Material Amber 400
    auto color =
        Quantity_Color(255 / 256.0, 202 / 256.0, 40 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(zRefPlanesN, color);
    shapes.push_back(comp);
  }

  TopoDS_Compound refPlanesAssy;
  BRep_Builder builder;
  builder.MakeCompound(refPlanesAssy);
  for (const TopoDS_Shape &shape : shapes) {
    builder.Add(refPlanesAssy, shape);
  }

  TDF_Label label = ctx->OCAFShapeTool()->AddShape(refPlanesAssy, true);
  TDataStd_Name::Set(label, "Reference Planes");

  OCX_INFO("Finished reading coordinate system...");

  return refPlanesAssy;
}

TopoDS_Shape OCXCoordinateSystemReader::ReadRefPlane(
    LDOM_Element const &refPlanesN, Quantity_Color const &color) {
  std::string refPlaneType = OCXHelper::GetLocalTagName(refPlanesN);

  OCX_INFO("Reading reference planes from {}", refPlaneType);

  int cntPlanes = 0;
  std::list<TopoDS_Shape> shapes;
  LDOM_Node aChildN = refPlanesN.getFirstChild();
  while (!aChildN.isNull()) {
    const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element refPlaneN = (LDOM_Element &)aChildN;

      char const *name = refPlaneN.getAttribute("name").GetString();
      char const *guid = refPlaneN.getAttribute(ctx->OCXGUIDRef()).GetString();

      if (std::string refPlaneName = OCXHelper::GetLocalTagName(refPlaneN);
          refPlaneName != "RefPlane") {
        OCX_ERROR(
            "Unexpected node type {} found in {}, expected node type to be "
            "RefPlane",
            refPlaneName, refPlaneType);
        aChildN = aChildN.getNextSibling();
        continue;
      }

      LDOM_Element refLocN =
          OCXHelper::GetFirstChild(refPlaneN, "ReferenceLocation");
      if (refLocN.isNull()) {
        OCX_ERROR(
            "No ReferenceLocation child node found in RefPlane {} guid={}",
            name, guid);
        aChildN = aChildN.getNextSibling();
        continue;
      }

      double location = OCXHelper::ReadDimension(refLocN, ctx);

      // Now create an OCC Plane
      double width = 50;
      double minX = -10;
      double maxX = 190;
      double minZ = -5;
      double maxZ = 45;

      gp_Pnt org;
      gp_Dir direction;
      gp_Pnt pnt0, pnt1, pnt2, pnt3;

      if (refPlaneType == "XRefPlanes") {
        // frames
        org = gp_Pnt(location, 0, 0);
        direction = gp_Dir(1, 0, 0);
        pnt0 = gp_Pnt(location, 1.10 * width / 2.0, minZ);
        pnt1 = gp_Pnt(location, -1.10 * width / 2.0, minZ);
        pnt2 = gp_Pnt(location, -1.10 * width / 2.0, maxZ);
        pnt3 = gp_Pnt(location, 1.10 * width / 2.0, maxZ);

      } else if (refPlaneType == "YRefPlanes") {
        // longitudinal
        org = gp_Pnt(0, location, 0);
        direction = gp_Dir(0, 1, 0);

        pnt0 = gp_Pnt(minX, location, minZ);
        pnt1 = gp_Pnt(maxX, location, minZ);
        pnt2 = gp_Pnt(maxX, location, maxZ);
        pnt3 = gp_Pnt(minX, location, maxZ);

      } else if (refPlaneType == "ZRefPlanes") {
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
      ctx->RegisterSurface(surface, guid);

      TDF_Label surfL = ctx->OCAFShapeTool()->AddShape(surface, false);
      TDataStd_Name::Set(surfL, name);
      ctx->OCAFColorTool()->SetColor(surfL, color, XCAFDoc_ColorSurf);

      shapes.push_back(surface);

      cntPlanes++;
    }
    aChildN = aChildN.getNextSibling();
  }

  TopoDS_Compound refPlanesXYZAssy;
  BRep_Builder refPlanesBuilder;
  refPlanesBuilder.MakeCompound(refPlanesXYZAssy);
  for (const TopoDS_Shape &shape : shapes) {
    refPlanesBuilder.Add(refPlanesXYZAssy, shape);
  }

  TDF_Label refSrfLabel =
      ctx->OCAFShapeTool()->AddShape(refPlanesXYZAssy, true);
  TDataStd_Name::Set(refSrfLabel, refPlaneType.c_str());

  OCX_INFO("Registered {} reference planes found in {}", cntPlanes,
           refPlaneType);

  return refPlanesXYZAssy;
}

}  // namespace ocx
