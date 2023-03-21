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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <GeomLProp_SLProps.hxx>


#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>

#include "ocx/internal/ocx-curve.h"
#include "ocx/internal/ocx-cut-by.h"
#include "ocx/ocx-helper.h"
#include <BRepPrimAPI_MakePrism.hxx>

namespace ocx::vessel::panel::cut_by {

std::list<TopoDS_Shape> ReadCutBy(LDOM_Element const &panelN) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element cutByN =
      ocx::helper::GetFirstChild(panelN, "CutBy");
  if (cutByN.isNull()) {
    OCX_WARN(
        "No CutBy child node found in ReadCutBy with panel id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = cutByN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element cutElementN = (LDOM_Element &)childN;

      if ((ocx::helper::GetLocalTagName(cutElementN) == "SlotContour") ||
          ocx::helper::GetLocalTagName(cutElementN) == "Hole2DContour") {
          if (TopoDS_Shape cutShape = ReadCutGeometry(cutElementN);
              !cutShape.IsNull()) {
            shapes.push_back(cutShape);
        }
      }
    }
    childN = childN.getNextSibling();
  }
  return shapes;
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadCutGeometry(LDOM_Element const &cutElementN) {
  auto meta = ocx::helper::GetOCXMeta(cutElementN);

  LDOM_Element holeRefele = ocx::helper::GetFirstChild(cutElementN, "HoleRef");

  if (holeRefele.isNull()) {
    OCX_ERROR(
        "No HoleRef child node found in ReadCutGeometry in CutBy "
        "id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  std::string guid = holeRefele.getAttribute("ocx:GUIDRef").GetString();
  // Find the shape of hole ref from Catalogue
  auto holeShape = ocx::OCXContext::GetInstance()->LookupHoleShape(guid);
  if (!holeShape.IsNull()) {
    // Get The Transformation
    LDOM_Element transformationEle =
        ocx::helper::GetFirstChild(cutElementN, "Transformation");
    gp_Trsf tra = ocx::helper::ReadTransformation(transformationEle);

    // Create The cutBy Geometry
    BRepBuilderAPI_MakeFace faceBuilder = BRepBuilderAPI_MakeFace(TopoDS::Wire(
        BRepBuilderAPI_Transform(holeShape, tra, true).Shape()));
    faceBuilder.Build();
    TopoDS_Face cutterFace = faceBuilder.Face();

    // Get The normal of the face
    Handle(Geom_Surface) surf =
        BRep_Tool::Surface(cutterFace);  // get surface properties
    GeomLProp_SLProps props(surf, 1, 1, 1, 0.01);
    gp_Dir norm = props.Normal();

    BRepPrimAPI_MakePrism solidBuilder =
        BRepPrimAPI_MakePrism(cutterFace, norm, false);
    solidBuilder.Build();

    return solidBuilder.Shape();
  }
  return TopoDS_Shape();
}

}  // namespace

}  // namespace ocx::vessel::panel::cut_by
