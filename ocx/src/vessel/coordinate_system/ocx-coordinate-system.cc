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

#include "ocx/internal/ocx-coordinate-system.h"

#include <occutils/occutils-wire.h>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pln.hxx>
#include <list>

#include "ocx/ocx-context.h"
#include "ocx/ocx-helper.h"

namespace ocx::vessel::coordinate_system {

void ReadCoordinateSystem(LDOM_Element const &vesselN) {
  OCX_INFO("Start reading coordinate system...")

  LDOM_Element cosysN = ocx::helper::GetFirstChild(vesselN, "CoordinateSystem");
  if (cosysN.isNull()) {
    OCX_ERROR("No CoordinateSystem child node found.")
    return;
  }

  // TODO: do we need to read the LocalCartesian ?
  // TODO: do we need to read the VesselGrid ?

  LDOM_Element frameTblsN = ocx::helper::GetFirstChild(cosysN, "FrameTables");
  if (frameTblsN.isNull()) {
    OCX_ERROR("No FrameTables child node found in CoordinateSystem")
    return;
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Element xRefPlanesN =
      ocx::helper::GetFirstChild(frameTblsN, "XRefPlanes");
  if (xRefPlanesN.isNull()) {
    OCX_ERROR("No XRefPlanes child node found in CoordinateSystem/FrameTables")
  } else {
    // Material Green 400
    auto color =
        Quantity_Color(102 / 256.0, 187 / 256.0, 106 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(xRefPlanesN, color);
    shapes.push_back(comp);
  }

  LDOM_Element yRefPlanesN =
      ocx::helper::GetFirstChild(frameTblsN, "YRefPlanes");
  if (yRefPlanesN.isNull()) {
    OCX_ERROR("No YRefPlanes child node found in CoordinateSystem/FrameTables")
  } else {
    // Material Lime 400
    auto color =
        Quantity_Color(212 / 256.0, 225 / 256.0, 87 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(yRefPlanesN, color);
    shapes.push_back(comp);
  }

  LDOM_Element zRefPlanesN =
      ocx::helper::GetFirstChild(frameTblsN, "ZRefPlanes");
  if (zRefPlanesN.isNull()) {
    OCX_ERROR("No ZRefPlanes child node found in CoordinateSystem/FrameTables")
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

  TDF_Label label =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(refPlanesAssy, true);
  TDataStd_Name::Set(label, "Reference Planes");

  OCX_INFO("Finished reading coordinate system...")
}

namespace {

TopoDS_Shape ReadRefPlane(LDOM_Element const &refPlanesN,
                          Quantity_Color const &color) {
  std::string refPlaneType = ocx::helper::GetLocalTagName(refPlanesN);

  OCX_INFO("Reading reference planes from {}", refPlaneType)

  int cntPlanes = 0;
  std::list<TopoDS_Shape> shapes;
  LDOM_Node aChildN = refPlanesN.getFirstChild();
  while (!aChildN.isNull()) {
    const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element refPlaneN = (LDOM_Element &)aChildN;
      auto meta = ocx::helper::GetOCXMeta(refPlaneN);

      if (std::string refPlaneName = ocx::helper::GetLocalTagName(refPlaneN);
          refPlaneName != "RefPlane") {
        OCX_ERROR(
            "Unexpected node type {} found in {}, expected node type to be "
            "RefPlane",
            refPlaneName, refPlaneType)
        aChildN = aChildN.getNextSibling();
        continue;
      }

      LDOM_Element refLocN =
          ocx::helper::GetFirstChild(refPlaneN, "ReferenceLocation");
      if (refLocN.isNull()) {
        OCX_ERROR(
            "No ReferenceLocation child node found in RefPlane {} guid={}",
            meta->name, meta->guid)
        aChildN = aChildN.getNextSibling();
        continue;
      }
      double location = ocx::helper::ReadDimension(refLocN);

      // Now create an OCC Plane
      double width = abs(OCXContext::MinY) + abs(OCXContext::MaxY);

      gp_Pnt org;
      gp_Dir direction;
      gp_Pnt pnt0, pnt1, pnt2, pnt3;

      if (refPlaneType == "XRefPlanes") {
        // YZ plane (frame)
        org = gp_Pnt(location, 0, 0);
        direction = gp_Dir(1, 0, 0);

        pnt0 = gp_Pnt(location, 1.10 * width / 2.0, OCXContext::MinZ);
        pnt1 = gp_Pnt(location, -1.10 * width / 2.0, OCXContext::MinZ);
        pnt2 = gp_Pnt(location, -1.10 * width / 2.0, OCXContext::MaxZ);
        pnt3 = gp_Pnt(location, 1.10 * width / 2.0, OCXContext::MaxZ);

      } else if (refPlaneType == "YRefPlanes") {
        // XZ plane (longitudinal)
        org = gp_Pnt(0, location, 0);
        direction = gp_Dir(0, 1, 0);

        pnt0 = gp_Pnt(OCXContext::MinX, location, OCXContext::MinZ);
        pnt1 = gp_Pnt(OCXContext::MaxX, location, OCXContext::MinZ);
        pnt2 = gp_Pnt(OCXContext::MaxX, location, OCXContext::MaxZ);
        pnt3 = gp_Pnt(OCXContext::MinX, location, OCXContext::MaxZ);

      } else if (refPlaneType == "ZRefPlanes") {
        // XY plane (deck)
        org = gp_Pnt(0, 0, location);
        direction = gp_Dir(0, 0, 1);

        pnt0 = gp_Pnt(OCXContext::MinX, 1.05 * width / 2.0, location);
        pnt1 = gp_Pnt(OCXContext::MinX, -1.05 * width / 2.0, location);
        pnt2 = gp_Pnt(OCXContext::MaxX, -1.05 * width / 2.0, location);
        pnt3 = gp_Pnt(OCXContext::MaxX, 1.05 * width / 2.0, location);
      }

      auto unlimitedSurface = gp_Pln(org, direction);
      TopoDS_Wire outerContour =
          OCCUtils::Wire::FromPoints({pnt0, pnt1, pnt2, pnt3}, true);
      if (!outerContour.Closed()) {
        OCX_ERROR(
            "Outer contour in ReadRefPlane is not closed. Skip building the "
            "RefPlane {} guid={}",
            meta->name, meta->guid)
        return {};
      }

      TopoDS_Face surface =
          BRepBuilderAPI_MakeFace(unlimitedSurface, outerContour);
      OCXContext::GetInstance()->RegisterShape(refPlaneN, surface);

      TDF_Label surfaceL =
          OCXContext::GetInstance()->OCAFShapeTool()->AddShape(surface, false);
      TDataStd_Name::Set(surfaceL, meta->name);
      OCXContext::GetInstance()->OCAFColorTool()->SetColor(surfaceL, color,
                                                           XCAFDoc_ColorSurf);

      shapes.push_back(surface);

      cntPlanes++;
    }
    aChildN = aChildN.getNextSibling();
  }

  TopoDS_Compound refPlanesAssy;
  BRep_Builder refPlanesBuilder;
  refPlanesBuilder.MakeCompound(refPlanesAssy);
  for (TopoDS_Shape const &shape : shapes) {
    refPlanesBuilder.Add(refPlanesAssy, shape);
  }

  TDF_Label refPlaneLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(refPlanesAssy, true);
  TDataStd_Name::Set(refPlaneLabel, refPlaneType.c_str());

  OCX_INFO("Registered {} reference planes found in {}", cntPlanes,
           refPlaneType)

  return refPlanesAssy;
}

}  // namespace

}  // namespace ocx::vessel::coordinate_system
