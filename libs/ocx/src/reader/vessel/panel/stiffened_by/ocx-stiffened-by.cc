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

#include "ocx/internal/ocx-stiffened-by.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>

#include "ocx/internal/ocx-curve.h"
#include "ocx/ocx-helper.h"

namespace ocx::reader::vessel::panel::stiffened_by {

TopoDS_Shape ReadStiffenedBy(LDOM_Element const &panelN, bool addShape) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  LDOM_Element stiffendedByN =
      ocx::helper::GetFirstChild(panelN, "StiffenedBy");
  if (stiffendedByN.isNull()) {
    OCX_ERROR(
        "No StiffenedBy child node found in ReadStiffenedBy with panel id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = stiffendedByN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element stiffenerN = (LDOM_Element &)childN;

      if (ocx::helper::GetLocalTagName(stiffenerN) == "Stiffener") {
        if (TopoDS_Shape stiffener = ReadStiffener(stiffenerN, addShape);
            !stiffener.IsNull()) {
          shapes.push_back(stiffener);
        }
      }
    }
    childN = childN.getNextSibling();
  }

  if (shapes.empty()) {
    OCX_WARN(
        "No Stiffener child node found in ReadStiffeners with panel id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  TopoDS_Compound stiffenersAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(stiffenersAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(stiffenersAssy, shape);
  }

  // No need to add shapes on first run if CreateLimitedBy is enabled
  if (!addShape) {
    return stiffenersAssy;
  }

  TDF_Label stiffenedByLabel =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(stiffenersAssy,
                                                           true);
  TDataStd_Name::Set(stiffenedByLabel, "StiffenedBy");

  return stiffenersAssy;
}

//-----------------------------------------------------------------------------

namespace {

TopoDS_Shape ReadStiffener(LDOM_Element const &stiffenerN, bool addShape) {
  auto meta = ocx::helper::GetOCXMeta(stiffenerN);

  LDOM_Element traceN = ocx::helper::GetFirstChild(stiffenerN, "TraceLine");
  // Also check for <ocx:TraceLine /> node
  if (traceN.isNull() || traceN.getFirstChild().isNull()) {
    OCX_ERROR(
        "No TraceLine child node found in ReadStiffener with stiffener "
        "id={} guid={}",
        meta->id, meta->guid)
    return {};
  }

  TopoDS_Shape trace = ocx::reader::shared::curve::ReadCurve(traceN);
  if (trace.IsNull()) {
    OCX_ERROR(
        "Failed to read TraceLine in ReadStiffener with stiffener id={} "
        "guid={}",
        meta->id, meta->guid)
    return {};
  }

  // Register trace line in the OCXContext
  OCXContext::GetInstance()->RegisterShape(stiffenerN, trace);

  // TODO: Add simplified geometry representation of stiffener

  // No need to add shapes on first run if CreateLimitedBy is enabled
  if (!addShape) {
    return trace;
  }

  // material design red 50 500
  auto plateColor =
      Quantity_Color(244 / 255.0, 67 / 255.0, 54 / 255.0, Quantity_TOC_RGB);

  TDF_Label label =
      OCXContext::GetInstance()->OCAFShapeTool()->AddShape(trace, false);
  TDataStd_Name::Set(label, ("Stiffener " + std::string(meta->id) + " (" +
                             std::string(meta->guid) + ")")
                                .c_str());
  OCXContext::GetInstance()->OCAFColorTool()->SetColor(label, plateColor,
                                                       XCAFDoc_ColorSurf);

  return trace;
}

}  // namespace

}  // namespace ocx::reader::vessel::panel::stiffened_by
