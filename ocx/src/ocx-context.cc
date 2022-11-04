//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-context.h"

#include <TDocStd_Document.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include "ocx/internal/ocx-helper.h"

OCXContext::OCXContext(const LDOM_Element &ocxDocN, const std::string &nsPrefix)
    : ocxDocN(ocxDocN), nsPrefix(nsPrefix) {
  this->ocxGUIDRef = LDOMString((nsPrefix + ":GUIDRef").c_str());
  this->ocxGUID = LDOMString((nsPrefix + ":GUID").c_str());
}

std::string OCXContext::Prefix() const { return nsPrefix; }

LDOMString OCXContext::OCXGUIDRef() const { return ocxGUIDRef; }

LDOMString OCXContext::OCXGUID() const { return ocxGUID; }

void OCXContext::PrepareUnits() {
  // Set main unit
  UnitsAPI::SetLocalSystem(UnitsAPI_SI);

  // Set defaults
  unit2factor["Um"] = 1;
  unit2factor["Udm"] = 0.1;
  unit2factor["Ucm"] = 0.01;
  unit2factor["Umm"] = 0.001;

  LDOM_Element unitsMLN = OCXHelper::GetFirstChild(ocxDocN, "UnitsML");
  if (unitsMLN.isNull()) {
    std::cout << "could not find UnitsML element" << std::endl;
    return;
  }

  LDOM_Node unitsSetN = OCXHelper::GetFirstChild(unitsMLN, "UnitSet");
  if (unitsSetN.isNull()) {
    std::cout << "could not find UnitsML/UnitSet element" << std::endl;
    return;
  }
  LDOM_Node aChildNode = unitsSetN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element unitN = (LDOM_Element &)aChildNode;
      if ("Unit" == OCXHelper::GetLocalTagName(unitN)) {
        LDOM_NodeList attributes = unitN.GetAttributesList();

        std::string id;
        for (int i = 0; i < attributes.getLength(); i++) {
          LDOM_Node theAtt = attributes.item(i);

          std::string name = OCXHelper::GetLocalAttrName(theAtt);

          if ("id" == name) {
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
}

double OCXContext::LoopupFactor(const std::string &unit) {
  if (auto res = unit2factor.find(unit); res != unit2factor.end()) {
    return res->second;
  }
  std::cout << "could not find factor for unit '" << unit << "', using 1.0"
            << std::endl;
  return 1;
}

void OCXContext::RegisterSurface(const std::string &guid,
                                 const TopoDS_Shape &shell) {
  if (shell.ShapeType() == TopAbs_SHELL || shell.ShapeType() == TopAbs_FACE) {
    guid2refplane[guid] = shell;
  } else {
    std::cerr << "RegisterSurfaces expects a SHELL or FACE, got type "
              << shell.ShapeType() << " for (" << guid << ")" << std::endl;
  }
}

TopoDS_Shape OCXContext::LookupSurface(const std::string &guid) {
  // TODO: check if exist
  return guid2refplane[guid];
}

void OCXContext::OCAFDoc(const opencascade::handle<TDocStd_Document> &handle) {
  ocafDoc = handle;
  ocafShapeTool = XCAFDoc_DocumentTool::ShapeTool(ocafDoc->Main());
  ocafColorTool = XCAFDoc_DocumentTool::ColorTool(ocafDoc->Main());
}

opencascade::handle<TDocStd_Document> OCXContext::OCAFDoc() { return ocafDoc; }

opencascade::handle<XCAFDoc_ShapeTool> OCXContext::OCAFShapeTool() {
  return ocafShapeTool;
}

opencascade::handle<XCAFDoc_ColorTool> OCXContext::OCAFColorTool() {
  return ocafColorTool;
}
