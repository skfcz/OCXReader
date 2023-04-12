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

#include "occutils/occutils-boolean.h"
#include "ocx/internal/ocx-cut-by.h"
#include "ocx/internal/ocx-unbounded-geometry.h"

namespace ocx::reader::vessel::panel::composed_of {

TopoDS_Shape ReadComposedOf(LDOM_Element const &panelN, bool withLimitedBy) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element composedOfN = ocx::helper::GetFirstChild(panelN, "ComposedOf");
  if (composedOfN.isNull()) {
    OCX_ERROR(
        "No ComposedOf child node found in ReadPlates with panel id={} guid={}",
        meta->id, meta->guid)
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
  auto plateMeta = ocx::helper::GetOCXMeta(plateN);
  auto panelMeta = ocx::helper::GetOCXMeta(plateN);

  std::list<TopoDS_Shape> shapes;

  // Snapshot of Panel and Plate Create-Options
  bool CreatePlateContours = OCXContext::CreatePlateContours;
  bool CreatePlateSurfaces = OCXContext::CreatePlateSurfaces;

  // Read the UnboundedGeometry, only do so if no reference surface for given
  // element can be found.
  TopoDS_Shape unboundedGeometryShape =
      OCXContext::GetInstance()->LookupShape(plateN);
  if (unboundedGeometryShape.IsNull()) {
    if (!ocx::helper::GetFirstChild(plateN, "UnboundedGeometry").isNull()) {
      TopoDS_Shape unboundedGeometry =
          ocx::reader::shared::unbounded_geometry::ReadUnboundedGeometry(
              plateN);
      if (!unboundedGeometry.IsNull()) {
        // Register the UnboundedGeometry in the context
        unboundedGeometryShape = unboundedGeometry;
        OCXContext::GetInstance()->RegisterShape(plateN,
                                                 unboundedGeometryShape);
      } else {
        OCX_ERROR(
            "Failed to read UnboundedGeometry element from Plate id={} guid={}",
            plateMeta->id, plateMeta->guid)
      }
    } else {
      OCX_WARN(
          "No UnboundedGeometry child node found in Plate element id={} "
          "guid={}. Try reading from parent UnboundedGeometry element.",
          plateMeta->id, plateMeta->guid)
      // Load it from the cache, as it should be parsed already
      unboundedGeometryShape = OCXContext::GetInstance()->LookupShape(panelN);
      if (!unboundedGeometryShape.IsNull()) {
        // Register the UnboundedGeometry in the context
        OCXContext::GetInstance()->RegisterShape(plateN,
                                                 unboundedGeometryShape);
      } else {
        OCX_ERROR("Failed to lookup parent ReferenceSurface id={} guid={}",
                  panelMeta->id, panelMeta->guid)
      }
    }
  }

  // Disable PlateSurfaces if enabled and no UnboundedGeometry is found
  if (unboundedGeometryShape.IsNull() && OCXContext::CreatePlateSurfaces) {
    OCX_WARN(
        "PlateSurfaces creation is enabled but belonging UnboundedGeometry "
        "is null. Disabling PlateSurfaces.")
    CreatePlateSurfaces = false;
  }

  // Read the Contour
  auto outerContour = TopoDS_Wire();  // retain scope for CreatePlateSurfaces
  if (CreatePlateContours && OCXContext::CreateLimitedBy == withLimitedBy) {
    outerContour = ocx::reader::shared::outer_contour::ReadOuterContour(plateN);
    if (!outerContour.IsNull()) {
      shapes.push_back(outerContour);
    } else {
      OCX_ERROR(
          "Failed to read OuterContour in ReadPlate with plate id={} guid={}",
          plateMeta->id, plateMeta->guid)

      // Disable PlateSurfaces if enabled
      if (OCXContext::CreatePlateSurfaces) {
        OCX_WARN(
            "PlateSurfaces creation is enabled but but PlateContours creation "
            "failed. Disabling PlateSurfaces.")
        CreatePlateSurfaces = false;
      }
    }
  }

  // Read the PlateSurface
  if (CreatePlateSurfaces && CreatePlateContours &&
      OCXContext::CreateLimitedBy == withLimitedBy) {
    TopoDS_Shape plateSurface = ocx::helper::LimitShapeByWire(
        unboundedGeometryShape, outerContour, plateMeta->id, plateMeta->guid);
    if (!plateSurface.IsNull()) {
      if (OCXContext::CreateCutBy) {
        // Apply CutBy geometries
        if (auto cutBy =
                ocx::vessel::panel::cut_by::ReadCutBy(panelN, plateSurface);
            !cutBy.IsNull()) {
          shapes.push_back(cutBy);
        }
      }

      // Material Design ...
      auto color =
          Quantity_Color(76 / 255.0, 175 / 255.0, 80 / 255.0, Quantity_TOC_RGB);
      TDF_Label plateSurfaceLabel =
          OCXContext::GetInstance()->OCAFShapeTool()->AddShape(plateSurface,
                                                               false);
      TDataStd_Name::Set(plateSurfaceLabel, "Surface");
      OCXContext::GetInstance()->OCAFColorTool()->SetColor(
          plateSurfaceLabel, color, XCAFDoc_ColorSurf);

      shapes.push_back(plateSurface);
    } else {
      OCX_ERROR(
          "Failed to create restricted surface (PlateSurface) in ReadPlate "
          "with plate id={} guid={}",
          plateMeta->id, plateMeta->guid)
    }
  }

  // Handle second run: LimitedBy nodes
  if (withLimitedBy) {
    TopoDS_Shape limitedBy =
        ocx::reader::shared::limited_by::ReadLimitedBy(plateN);
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
                     ("Plate (" + std::string(plateMeta->id) + ")").c_str());

  return plateAssy;
}

//-----------------------------------------------------------------------------

TopoDS_Shape ReadBracket(LDOM_Element const &bracketN) {
  OCX_WARN("Bracket not implemented yet")
  return {};
}

}  // namespace

}  // namespace ocx::reader::vessel::panel::composed_of
