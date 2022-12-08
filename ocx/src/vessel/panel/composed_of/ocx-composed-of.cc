/***************************************************************************
 *   Created on: 07 Dec 2022                                               *
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

#include "ocx/internal/ocx-composed-of.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>

#include "ocx/internal/ocx-limited-by.h"
#include "ocx/internal/ocx-outer-contour.h"
#include "ocx/internal/ocx-unbounded-geometry.h"

namespace ocx::vessel::panel::composed_of {

TopoDS_Shape ReadComposedOf(LDOM_Element const &panelN, bool withLimitedBy) {
  std::unique_ptr<ocx::helper::OCXMeta> meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element composedOfN = ocx::helper::GetFirstChild(panelN, "ComposedOf");
  if (composedOfN.isNull()) {
    OCX_ERROR(
        "No ComposedOf child node found in ReadPlates with panel id={} guid={}",
        meta->id, meta->guid);
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = composedOfN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element elementN = (LDOM_Element &)childN;

      if (ocx::helper::GetLocalTagName(elementN) == "Plate") {
        TopoDS_Shape plate = ReadPlate(panelN, elementN, withLimitedBy);
        if (!plate.IsNull()) {
          shapes.push_back(plate);
        }
      }
    }
    childN = childN.getNextSibling();
  }

  TopoDS_Compound composedOfAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(composedOfAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(composedOfAssy, shape);
  }

  // No need to add shapes on first run if CreateLimitedBy is enabled
  if (OCXContext::CreateLimitedBy && !withLimitedBy) {
    return composedOfAssy;
  }

  TDF_Label plateLabel = OCXContext::GetInstance()->OCAFShapeTool()->AddShape(
      composedOfAssy, true);
  TDataStd_Name::Set(plateLabel, "ComposedOf");

  return composedOfAssy;
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadPlate(LDOM_Element const &panelN, LDOM_Element const &plateN,
                       bool withLimitedBy) {
  std::unique_ptr<ocx::helper::OCXMeta> meta = ocx::helper::GetOCXMeta(plateN);

  std::list<TopoDS_Shape> shapes;

  // Snapshot of Panel and Plate Create-Options
  bool CreatePlateContours = OCXContext::CreatePlateContours;
  bool CreatePlateSurfaces = OCXContext::CreatePlateSurfaces;

  // Read the Contour
  auto outerContour = TopoDS_Wire();  // retain scope for CreatePlateSurfaces
  if (CreatePlateContours) {
    outerContour = ocx::shared::outer_contour::ReadOuterContour(
        plateN, OCXContext::CreateLimitedBy == withLimitedBy);
    if (!outerContour.IsNull()) {
      shapes.push_back(outerContour);
    }
  } else {
    OCX_ERROR(
        "Failed to read OuterContour in ReadPlate with plate id={} guid={}",
        meta->id, meta->guid);

    // Disable PlateSurfaces if enabled
    if (OCXContext::CreatePanelSurfaces) {
      OCX_WARN(
          "PlateSurfaces creation is enabled but but PlateContours creation "
          "failed. Disabling PlateSurfaces.");
      CreatePlateSurfaces = false;
    }
  }

  // Read the PlateSurface
  if (CreatePlateSurfaces) {
    TopoDS_Shape plateSurface = ReadPlateSurface(
        panelN, outerContour, OCXContext::CreateLimitedBy == withLimitedBy);
    if (!plateSurface.IsNull()) {
      // Register the restricted surface in the context
      OCXContext::GetInstance()->RegisterShape(
          plateN, ExtendedShape(plateSurface, "Plate"));

      shapes.push_back(plateSurface);
    } else {
      OCX_ERROR(
          "Failed to cut plate from Panel UnboundedGeometry in ReadPlate with "
          "plate id={} guid={}",
          meta->id, meta->guid);
    }
  }

  // Handle second run: LimitedBy nodes
  if (withLimitedBy) {
    TopoDS_Shape limitedBy = ocx::shared::limited_by::ReadLimitedBy(plateN);
    if (!limitedBy.IsNull()) {
      shapes.push_back(limitedBy);
    }
  }

  TopoDS_Compound plateAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(plateAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(plateAssy, shape);
  }

  // No need to add shapes on first run if CreateLimitedBy is enabled
  if (OCXContext::CreateLimitedBy && !withLimitedBy) {
    return plateAssy;
  }

  TDF_Label panelLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(plateAssy, true);
  TDataStd_Name::Set(panelLabel,
                     ("Plate (" + std::string(meta->id) + ")").c_str());

  return plateAssy;
}

TopoDS_Shape ReadBracket(LDOM_Element const &bracketN) {
  OCX_WARN("Bracket not implemented yet");
  return {};
}

TopoDS_Shape ReadPlateSurface(LDOM_Element const &elementN,
                              TopoDS_Wire const &outerContour, bool addShape) {
  std::unique_ptr<ocx::helper::OCXMeta> meta =
      ocx::helper::GetOCXMeta(elementN);

  // Read the UnboundedGeometry
  TopoDS_Shape unboundedGeometry =
      ocx::shared::unbounded_geometry::ReadUnboundedGeometry(elementN);
  if (unboundedGeometry.IsNull()) {
    OCX_ERROR("Failed to read UnboundedGeometry element id={} guid={}",
              meta->id, meta->guid);
    return {};
  }

  TopoDS_Shape surface = ocx::helper::CutShapeByWire(
      unboundedGeometry, outerContour, meta->id, meta->guid);
  if (surface.IsNull()) {
    OCX_ERROR("Failed to cut UnboundedGeometry element id={} guid={}", meta->id,
              meta->guid);
    return {};
  }

  if (!addShape) {
    return surface;
  }

  // Material Design ...
  auto color =
      Quantity_Color(76 / 255.0, 175 / 255.0, 80 / 255.0, Quantity_TOC_RGB);
  TDF_Label panelSurfaceLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(surface, false);
  TDataStd_Name::Set(panelSurfaceLabel, "Surface");
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(panelSurfaceLabel, color,
                                                       XCAFDoc_ColorSurf);

  return surface;
}

}  // namespace

}  // namespace ocx::vessel::panel::composed_of
