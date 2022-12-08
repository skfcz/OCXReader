/***************************************************************************
 *   Created on: 30 Nov 2022                                               *
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

#include "ocx/internal/ocx-limited-by.h"

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRep_Builder.hxx>
#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <memory>

#include "occutils/occutils-curve.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-util.h"

namespace ocx::shared::limited_by {

TopoDS_Shape ReadLimitedBy(LDOM_Element const &panelN) {
  std::unique_ptr<ocx::helper::OCXMeta> meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element limitedByN = ocx::helper::GetFirstChild(panelN, "LimitedBy");
  if (limitedByN.isNull()) {
    OCX_ERROR("No LimitedBy child node found in id={} guid={}", meta->id,
              meta->guid);
    return {};
  }

  std::list<TopoDS_Shape> limitedByShapes;

  LDOM_Node aChildNode = limitedByN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      auto limitedByShape = TopoDS_Shape();
      if (ocx::helper::GetLocalTagName(aElement) == "OcxItemPtr") {
        limitedByShape = ReadOcxItemPtr(panelN, aElement);
      } else if (ocx::helper::GetLocalTagName(aElement) == "FreeEdgeCurve3D") {
        limitedByShape = ReadFreeEdgeCurve3D(aElement);
      } else if (ocx::helper::GetLocalTagName(aElement) == "GridRef") {
        limitedByShape = ReadGridRef(panelN, aElement);
      } else {
        OCX_ERROR("Found unsupported LimitedBy child node {} in id={} guid={}",
                  meta->name, meta->id, meta->guid);
        aChildNode = aChildNode.getNextSibling();
        continue;
      }

      if (!limitedByShape.IsNull()) {
        limitedByShapes.push_back(limitedByShape);
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  if (limitedByShapes.empty()) {
    OCX_ERROR("No LimitedBy resolved in id={} guid={}", meta->id, meta->guid);
    return {};
  }

  TopoDS_Compound limitedByAssy;
  BRep_Builder limitedByBuilder;
  limitedByBuilder.MakeCompound(limitedByAssy);

  for (TopoDS_Shape const &shape : limitedByShapes) {
    limitedByBuilder.Add(limitedByAssy, shape);
  }

  // Add LimitedBy node in OCAF
  TDF_Label limitedByLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(limitedByAssy, true);
  TDataStd_Name::Set(limitedByLabel, "LimitedBy");

  return limitedByAssy;
}

namespace {

TopoDS_Shape ReadOcxItemPtr(LDOM_Element const &panelN,
                            LDOM_Element const &ocxItemPtrN) {
  std::unique_ptr<ocx::helper::OCXMeta> panelMeta =
      ocx::helper::GetOCXMeta(panelN);
  std::unique_ptr<ocx::helper::OCXMeta> ocxItemPtrMeta =
      ocx::helper::GetOCXMeta(ocxItemPtrN);

  ExtendedShape panelShape = OCXContext::GetInstance()->LookupShape(panelN);
  if (panelShape.m_shape.IsNull()) {
    OCX_ERROR("No Shape for given Panel found with id={} guid={}",
              panelMeta->id, panelMeta->guid);
    return {};
  }
  ExtendedShape ocxItemPtrShape =
      OCXContext::GetInstance()->LookupShape(ocxItemPtrN);
  if (ocxItemPtrShape.m_shape.IsNull()) {
    OCX_ERROR("No Shape for given OcxItemPtr found with guid={}",
              ocxItemPtrMeta->guid);
    return {};
  }

  // TODO: Only faces are supported for now
  if (!OCCUtils::Shape::IsFace(panelShape.m_shape) ||
      !OCCUtils::Shape::IsFace(ocxItemPtrShape.m_shape)) {
    OCX_ERROR("Only faces are supported for now");
    return {};
  }

  // Get intersection between Panel and referenced OcxItemPtr shape
  GeomAdaptor_Surface panelShapeAdapter =
      OCCUtils::Surface::FromFace(TopoDS::Face(panelShape.m_shape));
  GeomAdaptor_Surface ocxItemPtrShapeAdapter =
      OCCUtils::Surface::FromFace(TopoDS::Face(ocxItemPtrShape.m_shape));

  std::optional<TopoDS_Edge> limitedByShape =
      ocx::helper::Intersection(panelShapeAdapter, ocxItemPtrShapeAdapter);
  if (!limitedByShape.has_value()) {
    OCX_ERROR(
        "No intersection found between Panel id={} guid={} and "
        "OcxItemPtr guid={}",
        panelMeta->id, panelMeta->guid, ocxItemPtrMeta->guid);
    return {};
  }

  // Read bounding box coordinates
  LDOM_Element boundingBoxN =
      ocx::helper::GetFirstChild(ocxItemPtrN, "BoundingBox");
  if (boundingBoxN.isNull()) {
    OCX_ERROR("No BoundingBox child node found in OcxItemPtr guid={}",
              ocxItemPtrMeta->guid);
    return {};
  }

  gp_Pnt min =
      ocx::helper::ReadPoint(ocx::helper::GetFirstChild(boundingBoxN, "Min"));
  gp_Pnt max =
      ocx::helper::ReadPoint(ocx::helper::GetFirstChild(boundingBoxN, "Max"));
  // Expand the bounding box coordinates by 1mm (1^-3) to avoid numerical issues
  // when constructing the bounding box
  min.SetX(min.X() - 1e-3);
  min.SetY(min.Y() - 1e-3);
  min.SetZ(min.Z() - 1e-3);
  max.SetX(max.X() + 1e-3);
  max.SetY(max.Y() + 1e-3);
  max.SetZ(max.Z() + 1e-3);
  Bnd_Box boundingBox(min, max);

  // Limit the intersection edge to the bounding box
  GeomAdaptor_Curve limitedByShapeAdapter =
      OCCUtils::Curve::FromEdge(*limitedByShape);

  std::optional<TopoDS_Edge> limitedCurve =
      ocx::helper::CurveLimitByBoundingBox(limitedByShapeAdapter, boundingBox);
  if (!limitedCurve.has_value()) {
    OCX_ERROR(
        "Failed to limit the TopoDS_Edge by given BoundingBox with Panel id={} "
        "guid={} and OcxItemPtr guid={}",
        panelMeta->id, panelMeta->guid, ocxItemPtrMeta->guid);
  }

  // Add TopoDS_Edge to the OCAF
  TDF_Label limitedByL = OCXContext::GetInstance()->OCAFShapeTool()->AddShape(
      *limitedCurve, false);
  TDataStd_Name::Set(
      limitedByL,
      (ocxItemPtrShape.m_type + " " + ocxItemPtrMeta->guid).c_str());
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(
      limitedByL,
      Quantity_Color(20 / 256.0, 20 / 256.0, 20.0 / 256, Quantity_TOC_RGB),
      XCAFDoc_ColorCurv);

  return *limitedCurve;
}

//-----------------------------------------------------------------------------

TopoDS_Shape ReadFreeEdgeCurve3D(LDOM_Element const &curveN) {
  std::unique_ptr<ocx::helper::OCXMeta> meta = ocx::helper::GetOCXMeta(curveN);

  TopoDS_Shape curveShape = ocx::shared::curve::ReadCurve(curveN);
  if (curveShape.IsNull()) {
    OCX_ERROR("Failed to read FreeEdgeCurve3D with id={} guid={}", meta->id,
              meta->guid);
    return {};
  }

  // Add TopoDS_Edge to the OCAF
  TDF_Label limitedByL =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(curveShape, false);
  TDataStd_Name::Set(limitedByL,
                     ("FreeEdgeCurve3D " + std::string(meta->guid)).c_str());
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(
      limitedByL,
      Quantity_Color(20 / 256.0, 20 / 256.0, 20.0 / 256, Quantity_TOC_RGB),
      XCAFDoc_ColorCurv);

  return curveShape;
}

//-----------------------------------------------------------------------------

TopoDS_Shape ReadGridRef(LDOM_Element const &panelN,
                         LDOM_Element const &gridRefN) {
  std::unique_ptr<ocx::helper::OCXMeta> panelMeta =
      ocx::helper::GetOCXMeta(panelN);
  std::unique_ptr<ocx::helper::OCXMeta> gridRefMeta =
      ocx::helper::GetOCXMeta(gridRefN);

  ExtendedShape panelShape = OCXContext::GetInstance()->LookupShape(panelN);
  if (panelShape.m_shape.IsNull()) {
    OCX_ERROR("No Shape for given Panel found with id={} guid={}",
              panelMeta->id, panelMeta->guid);
    return {};
  }
  ExtendedShape gridRefShape = OCXContext::GetInstance()->LookupShape(gridRefN);
  if (gridRefShape.m_shape.IsNull()) {
    OCX_ERROR("No Shape for given GridRef found with guid={}",
              gridRefMeta->guid);
    return {};
  }

  // TODO: Only faces are supported for now
  if (!OCCUtils::Shape::IsFace(panelShape.m_shape) ||
      !OCCUtils::Shape::IsFace(gridRefShape.m_shape)) {
    OCX_ERROR("Only faces are supported for now");
    return {};
  }

  // Make new surface from the GridRef shape with given Offset
  // Read Offset
  LDOM_Element offsetN = ocx::helper::GetFirstChild(gridRefN, "Offset");
  if (offsetN.isNull()) {
    OCX_ERROR("No Offset child node found in GridRef guid={}",
              gridRefMeta->guid);
    return {};
  }
  double offset = ocx::helper::ReadDimension(offsetN);

  // Offset along the positive normal
  BRepOffsetAPI_MakeOffsetShape makeOffsetShape;
  makeOffsetShape.PerformBySimple(gridRefShape.m_shape, offset);
  if (!makeOffsetShape.IsDone()) {
    OCX_ERROR("Failed to offset GridRef with guid={}", gridRefMeta->guid);
    return {};
  }
  TopoDS_Shape gridRefOffsetShape = makeOffsetShape.Shape();

  // TODO: Maybe refactor into separate GridRef SurfaceRef functions as its also
  // TODO: used in shared/ocx-unbounded-geometry.cc

  // Get intersection between Panel and referenced OcxItemPtr shape
  GeomAdaptor_Surface panelShapeAdapter =
      OCCUtils::Surface::FromFace(TopoDS::Face(panelShape.m_shape));
  GeomAdaptor_Surface gridRefShapeAdapter =
      OCCUtils::Surface::FromFace(TopoDS::Face(gridRefOffsetShape));

  std::optional<TopoDS_Edge> limitedByShape =
      ocx::helper::Intersection(panelShapeAdapter, gridRefShapeAdapter);
  if (!limitedByShape.has_value()) {
    OCX_ERROR(
        "No intersection found between Panel id={} guid={} and "
        "GridRef guid={}",
        panelMeta->id, panelMeta->guid, gridRefMeta->guid);
    return {};
  }

  // Read bounding box coordinates
  LDOM_Element boundingBoxN =
      ocx::helper::GetFirstChild(gridRefN, "BoundingBox");
  if (boundingBoxN.isNull()) {
    OCX_ERROR("No BoundingBox child node found in GridRef guid={}",
              gridRefMeta->guid);
    return {};
  }

  gp_Pnt min =
      ocx::helper::ReadPoint(ocx::helper::GetFirstChild(boundingBoxN, "Min"));
  gp_Pnt max =
      ocx::helper::ReadPoint(ocx::helper::GetFirstChild(boundingBoxN, "Max"));
  // Expand the bounding box coordinates by 1mm (1^-3) to avoid numerical issues
  // when constructing the bounding box
  min.SetX(min.X() - 1e-3);
  min.SetY(min.Y() - 1e-3);
  min.SetZ(min.Z() - 1e-3);
  max.SetX(max.X() + 1e-3);
  max.SetY(max.Y() + 1e-3);
  max.SetZ(max.Z() + 1e-3);
  Bnd_Box boundingBox(min, max);

  // Limit the intersection edge to the bounding box
  GeomAdaptor_Curve limitedByShapeAdapter =
      OCCUtils::Curve::FromEdge(*limitedByShape);

  std::optional<TopoDS_Edge> limitedCurve =
      ocx::helper::CurveLimitByBoundingBox(limitedByShapeAdapter, boundingBox);
  if (!limitedCurve.has_value()) {
    OCX_ERROR(
        "Failed to limit the TopoDS_Edge by given BoundingBox with Panel id={} "
        "guid={} and GridRef guid={}",
        panelMeta->id, panelMeta->guid, gridRefMeta->guid);
  }

  // Add TopoDS_Edge to the OCAF
  TDF_Label limitedByL = OCXContext::GetInstance()->OCAFShapeTool()->AddShape(
      *limitedCurve, false);
  TDataStd_Name::Set(limitedByL,
                     (gridRefShape.m_type + " " + gridRefMeta->guid).c_str());
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(
      limitedByL,
      Quantity_Color(20 / 256.0, 20 / 256.0, 20.0 / 256, Quantity_TOC_RGB),
      XCAFDoc_ColorCurv);

  return *limitedCurve;
}

}  // namespace

}  // namespace ocx::shared::limited_by
