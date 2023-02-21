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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pln.hxx>
#include <list>
#include <magic_enum.hpp>

#include "occutils/occutils-wire.h"
#include "ocx/internal/ocx-vessel-grid-wrapper.h"
#include "ocx/ocx-context.h"

namespace ocx::reader::vessel::coordinate_system {

void ReadCoordinateSystem(LDOM_Element const &vesselN) {
  OCX_INFO("Start reading coordinate system...")

  LDOM_Element coordinateSystemN =
      ocx::helper::GetFirstChild(vesselN, "CoordinateSystem");
  if (coordinateSystemN.isNull()) {
    OCX_ERROR("No CoordinateSystem child node found.")
    return;
  }

  // TODO: do we need to read the LocalCartesian ?

  LDOM_Element frameTablesN =
      ocx::helper::GetFirstChild(coordinateSystemN, "FrameTables");
  if (frameTablesN.isNull()) {
    OCX_ERROR("No FrameTables child node found in CoordinateSystem")
    return;
  }

  std::list<TopoDS_Shape> refPlaneShapes;

  // TODO: Evaluate refactor with python like for "refplane", color in zip()

  if (LDOM_Element xRefPlanesN =
          ocx::helper::GetFirstChild(frameTablesN, "XRefPlanes");
      xRefPlanesN.isNull()) {
    OCX_ERROR("No XRefPlanes child node found in CoordinateSystem/FrameTables")
  } else {
    // Material Green 400
    auto color =
        Quantity_Color(102 / 256.0, 187 / 256.0, 106 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(xRefPlanesN, color);
    refPlaneShapes.push_back(comp);
  }

  if (LDOM_Element yRefPlanesN =
          ocx::helper::GetFirstChild(frameTablesN, "YRefPlanes");
      yRefPlanesN.isNull()) {
    OCX_ERROR("No YRefPlanes child node found in CoordinateSystem/FrameTables")
  } else {
    // Material Lime 400
    auto color =
        Quantity_Color(212 / 256.0, 225 / 256.0, 87 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(yRefPlanesN, color);
    refPlaneShapes.push_back(comp);
  }

  if (LDOM_Element zRefPlanesN =
          ocx::helper::GetFirstChild(frameTablesN, "ZRefPlanes");
      zRefPlanesN.isNull()) {
    OCX_ERROR("No ZRefPlanes child node found in CoordinateSystem/FrameTables")
  } else {
    // Material Amber 400
    auto color =
        Quantity_Color(255 / 256.0, 202 / 256.0, 40 / 256.0, Quantity_TOC_RGB);
    TopoDS_Shape comp = ReadRefPlane(zRefPlanesN, color);
    refPlaneShapes.push_back(comp);
  }

  TopoDS_Compound refPlanesAssy;
  BRep_Builder builder;
  builder.MakeCompound(refPlanesAssy);
  for (const TopoDS_Shape &shape : refPlaneShapes) {
    builder.Add(refPlanesAssy, shape);
  }

  TDF_Label label =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(refPlanesAssy, true);
  TDataStd_Name::Set(label, "Reference Planes");

  // TODO: Add configure option to enable/disable reading of VesselGrid
  // Read in VesselGrid
  if (LDOM_Element vesselGridN =
          ocx::helper::GetFirstChild(coordinateSystemN, "VesselGrid");
      vesselGridN.isNull()) {
    OCX_ERROR("No VesselGrid child node found.")
    return;
  } else {
    ReadVesselGrid(vesselGridN);
  }

  OCX_INFO("Finished reading coordinate system...")
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadRefPlane(LDOM_Element const &refPlanesN,
                          Quantity_Color const &color) {
  std::string refPlaneTypeName = ocx::helper::GetLocalTagName(refPlanesN);

  OCX_INFO("Reading reference planes from {}", refPlaneTypeName)

  int cntPlanes = 0;
  std::list<TopoDS_Shape> refPlaneShapes;
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
            refPlaneName, refPlaneTypeName)
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
      ocx::context_entities::RefPlaneType refPlaneType;

      if (refPlaneTypeName == "XRefPlanes") {
        // YZ plane (frame)
        org = gp_Pnt(location, 0, 0);
        direction = gp_Dir(1, 0, 0);

        pnt0 = gp_Pnt(location, 1.10 * width / 2.0, OCXContext::MinZ);
        pnt1 = gp_Pnt(location, -1.10 * width / 2.0, OCXContext::MinZ);
        pnt2 = gp_Pnt(location, -1.10 * width / 2.0, OCXContext::MaxZ);
        pnt3 = gp_Pnt(location, 1.10 * width / 2.0, OCXContext::MaxZ);
        refPlaneType = ocx::context_entities::RefPlaneType::X;

      } else if (refPlaneTypeName == "YRefPlanes") {
        // XZ plane (longitudinal)
        org = gp_Pnt(0, location, 0);
        direction = gp_Dir(0, 1, 0);

        pnt0 = gp_Pnt(OCXContext::MinX, location, OCXContext::MinZ);
        pnt1 = gp_Pnt(OCXContext::MaxX, location, OCXContext::MinZ);
        pnt2 = gp_Pnt(OCXContext::MaxX, location, OCXContext::MaxZ);
        pnt3 = gp_Pnt(OCXContext::MinX, location, OCXContext::MaxZ);
        refPlaneType = ocx::context_entities::RefPlaneType::Y;

      } else if (refPlaneTypeName == "ZRefPlanes") {
        // XY plane (deck)
        org = gp_Pnt(0, 0, location);
        direction = gp_Dir(0, 0, 1);

        pnt0 = gp_Pnt(OCXContext::MinX, 1.05 * width / 2.0, location);
        pnt1 = gp_Pnt(OCXContext::MinX, -1.05 * width / 2.0, location);
        pnt2 = gp_Pnt(OCXContext::MaxX, -1.05 * width / 2.0, location);
        pnt3 = gp_Pnt(OCXContext::MaxX, 1.05 * width / 2.0, location);
        refPlaneType = ocx::context_entities::RefPlaneType::Z;
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
      OCXContext::GetInstance()->RegisterRefPlane(
          meta->guid, refPlaneType, refPlaneN, direction, pnt0, pnt1, pnt3);

      TDF_Label surfaceL =
          OCXContext::GetInstance()->OCAFShapeTool()->AddShape(surface, false);
      TDataStd_Name::Set(surfaceL, meta->name);
      OCXContext::GetInstance()->OCAFColorTool()->SetColor(surfaceL, color,
                                                           XCAFDoc_ColorSurf);

      refPlaneShapes.push_back(surface);

      cntPlanes++;
    }
    aChildN = aChildN.getNextSibling();
  }

  TopoDS_Compound refPlanesAssy;
  BRep_Builder refPlanesBuilder;
  refPlanesBuilder.MakeCompound(refPlanesAssy);
  for (TopoDS_Shape const &shape : refPlaneShapes) {
    refPlanesBuilder.Add(refPlanesAssy, shape);
  }

  TDF_Label refPlaneLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(refPlanesAssy, true);
  TDataStd_Name::Set(refPlaneLabel, refPlaneTypeName.c_str());

  OCX_INFO("Registered {} reference planes found in {}", cntPlanes,
           refPlaneTypeName)

  return refPlanesAssy;
}

//-----------------------------------------------------------------------------

void ReadVesselGrid(LDOM_Element const &vesselGridN) {
  OCX_INFO("Reading VesselGrid")

  // Create a vector to store the VesselGridWrappers
  std::vector<context_entities::VesselGridWrapper> vesselGrid;

  for (auto const &gridType : {context_entities::VesselGridType::XGrid,
                               context_entities::VesselGridType::YGrid,
                               context_entities::VesselGridType::ZGrid}) {
    LDOM_Element gridN = ocx::helper::GetFirstChild(
        vesselGridN, magic_enum::enum_name(gridType));
    if (gridN.isNull()) {
      OCX_ERROR("No {} child node found in VesselGrid",
                magic_enum::enum_name(gridType))
      continue;
    }

    // Create VesselGridWrapper to store the grid data
    context_entities::VesselGridWrapper gridWrapper(gridType);

    if (std::string isReversed = ocx::helper::GetAttrValue(gridN, "isReversed");
        !isReversed.empty()) {
      gridWrapper.SetIsReversed(ocx::utils::stob(isReversed));
    }

    if (std::string isMainSystem =
            ocx::helper::GetAttrValue(gridN, "isMainSystem");
        !isMainSystem.empty()) {
      gridWrapper.SetIsMainSystem(ocx::utils::stob(isMainSystem));
    }

    if (LDOM_Element distanceToApN =
            ocx::helper::GetFirstChild(gridN, "DistanceToAP");
        !distanceToApN.isNull()) {
      gridWrapper.SetDistanceToAp(ocx::helper::ReadDimension(distanceToApN));
    }

    // Parse SpacingGroups
    LDOM_Node aChildN = gridN.getFirstChild();
    while (!aChildN.isNull()) {
      const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();
      if (aNodeType == LDOM_Node::ELEMENT_NODE) {
        LDOM_Element childN = (LDOM_Element &)aChildN;

        // Check for X,Y,Z-SpacingGroup
        if (ocx::helper::GetLocalTagName(childN).find("SpacingGroup") ==
            std::string::npos) {
          aChildN = aChildN.getNextSibling();
          continue;
        }

        Standard_Integer firstGridNumber, count;

        ocx::helper::GetIntegerAttribute(childN, "firstGridNumber",
                                         firstGridNumber);
        ocx::helper::GetIntegerAttribute(childN, "count", count);

        Standard_Real gridPosition, spacing;

        if (LDOM_Element gridPositionN =
                ocx::helper::GetFirstChild(childN, "GridPosition");
            !gridPositionN.isNull()) {
          gridPosition = ocx::helper::ReadDimension(gridPositionN);
        }

        if (LDOM_Element spacingN =
                ocx::helper::GetFirstChild(childN, "Spacing");
            !spacingN.isNull()) {
          spacing = ocx::helper::ReadDimension(spacingN);
        }

        // Add SpacingGroup to VesselGridWrapper
        gridWrapper.AddSpacingGroup(
            {firstGridNumber, count, spacing, gridPosition});
      }
      aChildN = aChildN.getNextSibling();
    }

    // Add VesselGridWrapper to vesselGrid
    vesselGrid.push_back(gridWrapper);
  }

  // Add VesselGridWrapper to OCXContext
  OCXContext::GetInstance()->RegisterVesselGrid(vesselGrid);
}

}  // namespace

}  // namespace ocx::reader::vessel::coordinate_system
