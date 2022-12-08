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

#include "ocx/internal/ocx-panel.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-log.h"
#include "ocx/internal/ocx-outer-contour.h"
#include "ocx/internal/ocx-stiffened-by.h"
#include "ocx/internal/ocx-unbounded-geometry.h"

namespace ocx::vessel::panel {

void ReadPanels(LDOM_Element const &vesselN) {
  OCX_INFO("Start reading panels...");

  // List containing the parsed panel assembly e.g. Contour, Surface,
  // ComposedOf, LimitedBy, etc.
  std::list<TopoDS_Shape> panels;

  // First run without LimitedBy nodes
  LDOM_Node aChildNode = vesselN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      if (ocx::helper::GetLocalTagName(aElement) == "Panel") {
        TopoDS_Shape panel = ReadPanel(aElement);
        if (!panel.IsNull() && !OCXContext::CreateLimitedBy) {
          panels.push_back(panel);
        }
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  if (panels.empty() && !OCXContext::CreateLimitedBy) {
    OCX_WARN("No panels found.");
    return;
  }

  // Second run with LimitedBy nodes
  if (OCXContext::CreateLimitedBy) {
    aChildNode = vesselN.getFirstChild();
    while (aChildNode != nullptr) {
      const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
      if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
      if (aNodeType == LDOM_Node::ELEMENT_NODE) {
        LDOM_Element aElement = (LDOM_Element &)aChildNode;

        if (ocx::helper::GetLocalTagName(aElement) == "Panel") {
          TopoDS_Shape panel = ReadPanel(aElement, true);
          if (!panel.IsNull()) {
            panels.push_back(panel);
          }
        }
      }
      aChildNode = aChildNode.getNextSibling();
    }
  }

  TopoDS_Compound panelsAssy;
  BRep_Builder panelsBuilder;
  panelsBuilder.MakeCompound(panelsAssy);

  for (TopoDS_Shape const &panel : panels) {
    panelsBuilder.Add(panelsAssy, panel);
  }

  TDF_Label panelsL =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(panelsAssy, true);
  TDataStd_Name::Set(panelsL, "Panels");

  OCX_INFO("Finished reading panels...");
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadPanel(LDOM_Element const &panelN, bool withLimitedBy) {
  std::unique_ptr<ocx::helper::OCXMeta> meta = ocx::helper::GetOCXMeta(panelN);

  std::list<TopoDS_Shape> shapes;

  // Snapshot of Panel and Plate Create-Options
  bool CreatePanelContours = OCXContext::CreatePanelContours;
  bool CreatePanelSurfaces = OCXContext::CreatePanelSurfaces;
  bool CreateComposedOf = OCXContext::CreateComposedOf;

  // Read the Contour
  auto outerContour = TopoDS_Wire();  // retain scope for CreatePanelSurfaces
  if (CreatePanelContours) {
    outerContour = ocx::shared::outer_contour::ReadOuterContour(
        panelN, OCXContext::CreateLimitedBy == withLimitedBy);
    if (!outerContour.IsNull()) {
      shapes.push_back(outerContour);
    }
  } else {
    OCX_ERROR(
        "Failed to read OuterContour in ReadPanel with panel id={} guid={}",
        meta->id, meta->guid);

    // Disable PanelSurfaces and PlateSurfaces if they are enabled
    if (OCXContext::CreatePanelSurfaces) {
      OCX_WARN(
          "PanelSurfaces creation is enabled but but PanelContours creation "
          "failed. Disabling PanelSurfaces.");
      CreatePanelSurfaces = false;
    }
  }

  // TODO: Read CutBy here

  // Read ComposedOf
  if (CreateComposedOf) {
    TopoDS_Shape composedOf =
        ocx::vessel::panel::composed_of::ReadComposedOf(panelN, withLimitedBy);
    if (!composedOf.IsNull()) {
      shapes.push_back(composedOf);
    } else {
      OCX_ERROR(
          "Failed to read ComposedOf in ReadPanel with panel id={} guid={}",
          meta->id, meta->guid);
    }
  }

  // Create the resulting PanelSurface (UnboundedGeometry restricted by
  // OuterContour and CutBy, where OuterContour is required)
  auto panelSurface = TopoDS_Shape();  // retain scope for CreatePlateSurfaces
  if (CreatePanelSurfaces) {
    panelSurface = ReadPanelSurface(
        panelN, outerContour, OCXContext::CreateLimitedBy == withLimitedBy);
    if (!panelSurface.IsNull()) {
      // Register the restricted surface in the context
      OCXContext::GetInstance()->RegisterShape(
          panelN, ExtendedShape(panelSurface, "Panel"));

      shapes.push_back(panelSurface);
    } else {
      OCX_ERROR(
          "Failed to read PanelSurface in ReadPanel with panel id={} guid={}",
          meta->id, meta->guid);
    }
  }

  // Read StiffenedBy
  if (OCXContext::CreateStiffenerTraces) {
    TopoDS_Shape stiffenedBy =
        ocx::vessel::panel::stiffened_by::ReadStiffenedBy(
            panelN, OCXContext::CreateLimitedBy == withLimitedBy);
    if (!stiffenedBy.IsNull()) {
      shapes.push_back(stiffenedBy);
    }
  }

  // Handle second run: LimitedBy nodes
  if (withLimitedBy) {
    TopoDS_Shape limitedBy = ocx::shared::limited_by::ReadLimitedBy(panelN);
    if (!limitedBy.IsNull()) {
      shapes.push_back(limitedBy);
    }
  }

  // No need to add shapes on first run if CreateLimitedBy is enabled
  if (OCXContext::CreateLimitedBy && !withLimitedBy) {
    return {};
  }

  TopoDS_Compound panelAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(panelAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(panelAssy, shape);
  }

  TDF_Label panelLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(panelAssy, true);
  TDataStd_Name::Set(panelLabel, ("Panel " + std::string(meta->name) + " (" +
                                  std::string(meta->id) + ")")
                                     .c_str());

  return panelAssy;
}

//-----------------------------------------------------------------------------

TopoDS_Shape ReadPanelSurface(LDOM_Element const &elementN,
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

  // Material Design Light Green 50 300
  auto color =
      Quantity_Color(174 / 256.0, 213 / 256.0, 129.0 / 256, Quantity_TOC_RGB);
  TDF_Label panelSurfaceLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(surface, false);
  TDataStd_Name::Set(panelSurfaceLabel, "Surface");
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(panelSurfaceLabel, color,
                                                       XCAFDoc_ColorSurf);

  return surface;
}

}  // namespace

}  // namespace ocx::vessel::panel
