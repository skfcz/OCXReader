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

#include "ocx/internal/ocx-context.h"

#include <occutils/occutils-shape.h>

#include <LDOM_Element.hxx>
#include <TDocStd_Document.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <memory>
#include <utility>

#include "ocx/internal/ocx-exceptions.h"
#include "ocx/internal/ocx-helper.h"

namespace ocx {

bool LDOMCompare::operator()(LDOM_Element const &lhs,
                             LDOM_Element const &rhs) const {
  std::unique_ptr<ocx::helper::OCXMeta> metaLhs = ocx::helper::GetOCXMeta(lhs);
  std::unique_ptr<ocx::helper::OCXMeta> metaRhs = ocx::helper::GetOCXMeta(rhs);

  if (metaLhs->guid != nullptr && metaRhs->guid != nullptr) {
    return strcmp(metaLhs->guid, metaRhs->guid) < 0;
  } else if (metaLhs->id != nullptr && metaRhs->id != nullptr) {
    return strcmp(metaLhs->id, metaRhs->id) < 0;
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

OCXContext::OCXContext(LDOM_Element const &root, std::string nsPrefix)
    : m_root(root), m_nsPrefix(std::move(nsPrefix)) {}

//-----------------------------------------------------------------------------

std::shared_ptr<OCXContext> OCXContext::s_instance = nullptr;

//-----------------------------------------------------------------------------

void OCXContext::Initialize(LDOM_Element const &root,
                            std::string const &nsPrefix) {
  if (s_instance == nullptr) {
    if (root != nullptr) {
      s_instance = create(root, nsPrefix);
    } else {
      throw OCXInitializationFailedException(
          "OCXContext initialization failed");
    }
  } else {
    OCX_ERROR("OCXContext already initialized");
  }
}

//-----------------------------------------------------------------------------

std::shared_ptr<OCXContext> OCXContext::GetInstance() {
  if (s_instance == nullptr) {
    throw OCXNotFoundException("OCXContext not initialized");
  }
  return s_instance;
}

//-----------------------------------------------------------------------------

LDOM_Element OCXContext::OCXRoot() const { return m_root; }

//-----------------------------------------------------------------------------

std::string OCXContext::Prefix() const { return m_nsPrefix; }

//-----------------------------------------------------------------------------

void OCXContext::PrepareUnits() {
  OCX_INFO("Setting up units...");

  // Set main unit
  UnitsAPI::SetLocalSystem(UnitsAPI_SI);

  // Set defaults
  unit2factor["Um"] = 1;
  unit2factor["Udm"] = 0.1;
  unit2factor["Ucm"] = 0.01;
  unit2factor["Umm"] = 0.001;

  LDOM_Element unitsMLN = ocx::helper::GetFirstChild(m_root, "UnitsML");
  if (unitsMLN.isNull()) {
    OCX_ERROR("No UnitsML node found");
    return;
  }

  LDOM_Node unitsSetN = ocx::helper::GetFirstChild(unitsMLN, "UnitSet");
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
      if (ocx::helper::GetLocalTagName(unitN) == "Unit") {
        LDOM_NodeList attributes = unitN.GetAttributesList();

        std::string id;
        for (int i = 0; i < attributes.getLength(); i++) {
          LDOM_Node theAtt = attributes.item(i);

          if (ocx::helper::GetLocalAttrName(theAtt) == "id") {
            id = std::string(theAtt.getNodeValue().GetString());
          }
        }

        std::string symbol = ocx::helper::GetFirstChild(unitN, "UnitSymbol")
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

//-----------------------------------------------------------------------------

double OCXContext::LoopupFactor(const std::string &unit) const {
  if (auto res = unit2factor.find(unit); res != unit2factor.end()) {
    return res->second;
  }
  OCX_INFO("No factor found for unit {}, using 1.0 instead", unit);
  return 1;
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterShape(LDOM_Element const &element,
                               TopoDS_Shape const &shape) {
  LDOM2TopoDS_Shape[element] = shape;
}

TopoDS_Shape OCXContext::LookupShape(LDOM_Element const &element) {
  if (auto res = LDOM2TopoDS_Shape.find(element);
      res != LDOM2TopoDS_Shape.end()) {
    return res->second;
  }
  OCX_WARN("No TopoDS_Shape found for given LDOM_Element");
  return {};
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterBarSection(LDOM_Element const &element,
                                    BarSection const &section) {
  LDOM2BarSection[element] = section;
}

BarSection OCXContext::LookupBarSection(LDOM_Element const &element) const {
  if (auto res = LDOM2BarSection.find(element); res != LDOM2BarSection.end()) {
    return res->second;
  }
  OCX_ERROR("No Shape found for given LDOM_Element");
  return {};
}

//-----------------------------------------------------------------------------

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
