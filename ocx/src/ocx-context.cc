// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-context.h"

#include <occutils/occutils-shape.h>

#include <TDocStd_Document.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include "ocx/internal/ocx-helper.h"

namespace ocx {

std::string OCXContext::Prefix() const { return nsPrefix; }

LDOMString OCXContext::OCXGUIDRef() const { return ocxGUIDRef; }

LDOMString OCXContext::OCXGUID() const { return ocxGUID; }

void OCXContext::PrepareUnits() {
  OCX_INFO("Setting up units...");

  // Set main unit
  UnitsAPI::SetLocalSystem(UnitsAPI_SI);

  // Set defaults
  unit2factor["Um"] = 1;
  unit2factor["Udm"] = 0.1;
  unit2factor["Ucm"] = 0.01;
  unit2factor["Umm"] = 0.001;

  LDOM_Element unitsMLN = OCXHelper::GetFirstChild(ocxDocN, "UnitsML");
  if (unitsMLN.isNull()) {
    OCX_ERROR("No UnitsML node found");
    return;
  }

  LDOM_Node unitsSetN = OCXHelper::GetFirstChild(unitsMLN, "UnitSet");
  if (unitsSetN.isNull()) {
    OCX_ERROR("No UnitSet node found in UnitsML");
    return;
  }

  LDOM_Node aChildNode = unitsSetN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element unitN = (LDOM_Element &)aChildNode;
      if (OCXHelper::GetLocalTagName(unitN) == "Unit") {
        LDOM_NodeList attributes = unitN.GetAttributesList();

        std::string id;
        for (int i = 0; i < attributes.getLength(); i++) {
          LDOM_Node theAtt = attributes.item(i);

          if (OCXHelper::GetLocalAttrName(theAtt) == "id") {
            id = std::string(theAtt.getNodeValue().GetString());
          }
        }

        std::string symbol = OCXHelper::GetFirstChild(unitN, "UnitSymbol")
                                 .getAttribute("type")
                                 .GetString();

        if ("m" == symbol) {
          unit2factor[id] = 1;
        } else if ("dm" == symbol) {
          unit2factor[id] = 1 / 10.0;
        } else if ("cm" == symbol) {
          unit2factor[id] = 1 / 100.0;
        } else if ("mm" == symbol) {
          unit2factor[id] = 1 / 1000.0;
        }
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  OCX_INFO("Units set up successfully...");
}

double OCXContext::LoopupFactor(const std::string &unit) const {
  if (auto res = unit2factor.find(unit); res != unit2factor.end()) {
    return res->second;
  }
  OCX_INFO("No factor found for unit {}, using 1.0 instead", unit);
  return 1;
}

void OCXContext::RegisterSurface(TopoDS_Shape const &shape,
                                 std::string const &guid) {
  if (OCCUtils::Shape::IsFace(shape) || OCCUtils::Shape::IsShell(shape)) {
    guid2refPlane[guid] = shape;
  } else {
    OCX_ERROR(
        "Trying to register a non-shell. Expected a TopAbs_SHELL or a "
        "TopAbs_FACE, but got type {} for guid={}",
        shape.ShapeType(), guid);
  }
}

TopoDS_Shape OCXContext::LookupSurface(
    std::basic_string<char> const &guid) const {
  if (auto res = guid2refPlane.find(guid); res != guid2refPlane.end()) {
    return res->second;
  }
  OCX_ERROR("No registered surface found for guid={}", guid);
  return {};
}

void OCXContext::OCAFDoc(const opencascade::handle<TDocStd_Document> &handle) {
  ocafDoc = handle;
  ocafShapeTool = XCAFDoc_DocumentTool::ShapeTool(ocafDoc->Main());
  ocafColorTool = XCAFDoc_DocumentTool::ColorTool(ocafDoc->Main());
}

opencascade::handle<TDocStd_Document> OCXContext::OCAFDoc() const {
  return ocafDoc;
}

opencascade::handle<XCAFDoc_ShapeTool> OCXContext::OCAFShapeTool() const {
  return ocafShapeTool;
}

opencascade::handle<XCAFDoc_ColorTool> OCXContext::OCAFColorTool() const {
  return ocafColorTool;
}

}  // namespace ocx
