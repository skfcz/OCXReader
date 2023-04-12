/***************************************************************************
 *   Created on: 02 Dec 2022                                               *
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

#include "ocx/internal/ocx-cut-by.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Builder.hxx>
#include <GeomLProp_SLProps.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <vector>

#include "occutils/occutils-boolean.h"
#include "ocx/ocx-helper.h"

namespace ocx::vessel::panel::cut_by {

TopoDS_Shape ReadCutBy(LDOM_Element const &panelN, TopoDS_Shape &panelShape) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element cutByN = ocx::helper::GetFirstChild(panelN, "CutBy");
  if (cutByN.isNull()) {
    OCX_WARN(
        "No CutBy child node found in ReadCutBy with panel id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  std::vector<TopoDS_Shape> shapes;

  LDOM_Node childN = cutByN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element cutElementN = (LDOM_Element &)childN;

      auto holeContourMeta = ocx::helper::GetOCXMeta(cutElementN);
      if (auto contourType = ocx::helper::GetLocalTagName(cutElementN);
          contourType == "SlotContour" || contourType == "Hole2DContour") {
        if (TopoDS_Wire cutContour = ReadCutGeometry(cutElementN);
            !cutContour.IsNull()) {
          // Material Design ...
          auto cutContourColor = Quantity_Color(20 / 256.0, 20 / 256.0,
                                                20.0 / 256, Quantity_TOC_RGB);

          // Add Contour node in OCAF
          TDF_Label cutContourLabel =
              OCXContext::GetInstance()->OCAFShapeTool()->AddShape(cutContour,
                                                                   false);
          TDataStd_Name::Set(
              cutContourLabel,
              ("Contour (" + std::string(holeContourMeta->id) + ")").c_str());
          OCXContext::GetInstance()->OCAFColorTool()->SetColor(
              cutContourLabel, cutContourColor, XCAFDoc_ColorCurv);

          // Create the solid for cutting
          BRepBuilderAPI_MakeFace faceBuilder =
              BRepBuilderAPI_MakeFace(cutContour);
          TopoDS_Face const &cutFace = faceBuilder.Face();

          // Get the normal of the face
          Handle(Geom_Surface) surface = BRep_Tool::Surface(cutFace);
          GeomLProp_SLProps surfaceProps(surface, 1, 1, 1, 0.01);
          gp_Dir normalDirection = surfaceProps.Normal();

          // Create the solid for cutting
          TopoDS_Shape cutShape;
          try {
            auto solidBuilder =
                BRepPrimAPI_MakePrism(cutFace, normalDirection, false);
            solidBuilder.Build();
            cutShape = solidBuilder.Shape();
          } catch (StdFail_NotDone &e) {
            OCX_ERROR(
                "Failed to create cut geometry in ReadCutBy with hole contour "
                "id={} guid={}: {}",
                holeContourMeta->id, holeContourMeta->guid,
                e.GetMessageString())
            shapes.push_back(cutContour);
            childN = childN.getNextSibling();
            continue;
          }

          try {
            panelShape = OCCUtils::Boolean::Cut(panelShape, cutShape);
          } catch (StdFail_NotDone &e) {
            OCX_ERROR(
                "Failed to cut panel surface with cut geometry in "
                "ReadCutBy with hole contour id={} guid={}: {}",
                holeContourMeta->id, holeContourMeta->guid,
                e.GetMessageString())
          }

          shapes.push_back(cutContour);
        }
      }
    }
    childN = childN.getNextSibling();
  }

  TopoDS_Compound cutByAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(cutByAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(cutByAssy, shape);
  }

  TDF_Label cutByLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(cutByAssy, true);
  TDataStd_Name::Set(cutByLabel, "CutBy");

  return cutByAssy;
}

//-----------------------------------------------------------------------------

namespace {
TopoDS_Wire ReadCutGeometry(LDOM_Element const &cutElementN) {
  auto meta = ocx::helper::GetOCXMeta(cutElementN);

  LDOM_Element holeRef = ocx::helper::GetFirstChild(cutElementN, "HoleRef");
  if (holeRef.isNull()) {
    OCX_ERROR(
        "No HoleRef child node found in ReadCutGeometry in CutBy "
        "id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  // Find the shape of the hole reference from the Catalogue
  auto holeRefMeta = ocx::helper::GetOCXMeta(holeRef);
  auto holeShape =
      ocx::OCXContext::GetInstance()->LookupHoleShape(holeRefMeta->guid);

  if (!holeShape.IsNull()) {
    // Get the transformation element
    LDOM_Element transformationEle =
        ocx::helper::GetFirstChild(cutElementN, "Transformation");

    // Read the transformation
    gp_Trsf localToGlobalTrsf =
        ocx::helper::ReadTransformation(transformationEle);

    try {
      auto transformedHoleShape =
          BRepBuilderAPI_Transform(holeShape, localToGlobalTrsf, true).Shape();
      return TopoDS::Wire(transformedHoleShape);
    } catch (StdFail_NotDone &e) {
      OCX_ERROR(
          "Failed to transform HoleShape in ReadCutGeometry in CutBy "
          "for HoleRef id={} guid={}: {}",
          holeRefMeta->id, holeRefMeta->guid, e.GetMessageString())
      return {};
    }
  }

  OCX_ERROR(
      "No HoleShape found in ReadCutGeometry in CutBy "
      "for HoleRef id={} guid={}",
      holeRefMeta->id, holeRefMeta->guid)

  return {};
}

}  // namespace

}  // namespace ocx::vessel::panel::cut_by
