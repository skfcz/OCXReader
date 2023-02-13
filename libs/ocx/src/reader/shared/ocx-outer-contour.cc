/***************************************************************************
 *   Created on: 06 Dec 2022                                               *
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

#include "ocx/internal/ocx-outer-contour.h"

namespace ocx::reader::shared::outer_contour {

TopoDS_Wire ReadOuterContour(LDOM_Element const& elementN) {
  auto meta = ocx::helper::GetOCXMeta(elementN);

  LDOM_Element outerContourN =
      ocx::helper::GetFirstChild(elementN, "OuterContour");
  if (outerContourN.isNull()) {
    OCX_ERROR("No OuterContour child node found in element id={} guid={}",
              meta->id, meta->guid)
    return {};
  }

  TopoDS_Shape curveShape =
      ocx::reader::shared::curve::ReadCurve(outerContourN);
  if (curveShape.IsNull()) {
    OCX_ERROR("Failed to read curve shape from element id={} guid={}", meta->id,
              meta->guid)
    return {};
  }
  if (!OCCUtils::Shape::IsWire(curveShape)) {
    OCX_ERROR(
        "OuterContour child node in element id={} guid={} is not of type "
        "TopoDS_Wire, expected a closed edge as OuterContour, but got {}",
        meta->id, meta->guid, curveShape.ShapeType())
    return {};
  }

  // Material Design ...
  auto contourColor =
      Quantity_Color(20 / 256.0, 20 / 256.0, 20.0 / 256, Quantity_TOC_RGB);

  // Add Contour node in OCAF
  TDF_Label contourLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(curveShape, false);
  TDataStd_Name::Set(contourLabel, "Contour");
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(
      contourLabel, contourColor, XCAFDoc_ColorCurv);

  return TopoDS::Wire(curveShape);
}

}  // namespace ocx::reader::shared::outer_contour
