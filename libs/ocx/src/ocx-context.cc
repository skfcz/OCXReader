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

#include "ocx/ocx-context.h"

#include <LDOM_Element.hxx>
#include <TDocStd_Document.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <memory>
#include <utility>

#include "occutils/occutils-shape.h"
#include "ocx/internal/ocx-bar-section.h"
#include "ocx/internal/ocx-exceptions.h"
#include "ocx/internal/ocx-refplane-wrapper.h"
#include "ocx/ocx-helper.h"

namespace ocx {

bool LDOMCompare::operator()(LDOM_Element const &lhs,
                             LDOM_Element const &rhs) const {
  auto metaLhs = ocx::helper::GetOCXMeta(lhs);
  auto metaRhs = ocx::helper::GetOCXMeta(rhs);

  if (metaLhs->guid != nullptr && metaRhs->guid != nullptr) {
    return strcmp(metaLhs->guid, metaRhs->guid) < 0;
  } else if (metaLhs->id != nullptr && metaRhs->id != nullptr) {
    return strcmp(metaLhs->id, metaRhs->id) < 0;
  } else if (metaLhs->localRef != nullptr && metaRhs->localRef != nullptr) {
    return strcmp(metaLhs->localRef, metaRhs->localRef) < 0;
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
    OCX_ERROR("OCXContext already initialized")
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
  OCX_INFO("Setting up units...")

  // Set main unit
  UnitsAPI::SetLocalSystem(UnitsAPI_SI);

  // Set defaults
  unit2factor["Um"] = 1;
  unit2factor["Udm"] = 0.1;
  unit2factor["Ucm"] = 0.01;
  unit2factor["Umm"] = 0.001;

  LDOM_Element unitsMLN = ocx::helper::GetFirstChild(m_root, "UnitsML");
  if (unitsMLN.isNull()) {
    OCX_ERROR("No UnitsML node found")
    return;
  }

  LDOM_Node unitsSetN = ocx::helper::GetFirstChild(unitsMLN, "UnitSet");
  if (unitsSetN.isNull()) {
    OCX_ERROR("No UnitSet node found in UnitsML")
    return;
  }

  LDOM_Node childN = unitsSetN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element unitN = (LDOM_Element &)childN;
      if (ocx::helper::GetLocalTagName(unitN) == "Unit") {
        // Parse unit ID
        std::string unitId = unitN.getAttribute("xml:id").GetString();
        if (unitId.empty()) {
          OCX_ERROR("No xml:id attribute found in Unit node")
          childN = childN.getNextSibling();
          continue;
        }

        // Parse unit symbol
        LDOM_Element unitSymbolN =
            ocx::helper::GetFirstChild(unitN, "UnitSymbol");
        if (unitSymbolN.isNull()) {
          OCX_ERROR("No UnitSymbol child node found in Unit")
          childN = childN.getNextSibling();
          continue;
        }
        std::string unitSymbol = unitSymbolN.getAttribute("type").GetString();

        OCX_INFO("UnitID={}, UnitSymbol={}", unitId, unitSymbol)

        // Add unit to map
        if ("m" == unitSymbol) {
          unit2factor[unitId] = 1;
        } else if ("dm" == unitSymbol) {
          unit2factor[unitId] = 1 / 10.0;
        } else if ("cm" == unitSymbol) {
          unit2factor[unitId] = 1 / 100.0;
        } else if ("mm" == unitSymbol) {
          unit2factor[unitId] = 1 / 1000.0;
        }
      }
    }
    childN = childN.getNextSibling();
  }

  OCX_INFO("Units set up successfully...")
}

//-----------------------------------------------------------------------------

double OCXContext::LoopupFactor(const std::string &unit) const {
  if (auto res = unit2factor.find(unit); res != unit2factor.end()) {
    return res->second;
  }
  OCX_INFO("No loopup-factor found for unit {}, using 1.0 instead", unit)
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
  return {};
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterRefPlane(
    std::string const &guid, ocx::context_entities::RefPlaneType const &type,
    LDOM_Element const &element, gp_Dir const &normal, gp_Pnt const &p0,
    gp_Pnt const &p1, gp_Pnt const &p2) {
  GUID2RefPlane[guid] = ocx::context_entities::RefPlaneWrapper(
      guid, type, element, normal, p0, p1, p2);
}

ocx::context_entities::RefPlaneWrapper OCXContext::LookupRefPlane(
    std::string_view const &guid) {
  if (auto res = GUID2RefPlane.find(guid); res != GUID2RefPlane.end()) {
    return res->second;
  }
  OCX_ERROR("No RefPlane found for given guid {}", guid)
  return {};
}
//-----------------------------------------------------------------------------

void OCXContext::RegisterHoleShape(std::string const &guid,
                                   TopoDS_Shape const &holeShape) {
  m_holeCatalogue[guid] = holeShape;
}

//-----------------------------------------------------------------------------

TopoDS_Shape OCXContext::LookupHoleShape(std::string_view const &guid) {
  if (auto res = m_holeCatalogue.find(guid); res != m_holeCatalogue.end()) {
    return res->second;
  }
  OCX_ERROR("No HoleShape found for given guid {}", guid)
  return {};
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterPrincipalParticulars(
    ocx::context_entities::PrincipalParticularsWrapper const
        &PrincipalParticularsWrapper) {
  m_principalParticulars = PrincipalParticularsWrapper;
}

ocx::context_entities::PrincipalParticularsWrapper
OCXContext::GetPrincipalParticulars() {
  return m_principalParticulars;
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterVesselGrid(
    std::vector<ocx::context_entities::VesselGridWrapper> const
        &vesselGridWrappers) {
  m_vesselGrid = vesselGridWrappers;
}

std::vector<ocx::context_entities::VesselGridWrapper>
OCXContext::GetVesselGrid() {
  return m_vesselGrid;
}

//-----------------------------------------------------------------------------

void OCXContext::RegisterBarSection(
    LDOM_Element const &element,
    ocx::context_entities::BarSection const &section) {
  LDOM2BarSection[element] = section;
}

ocx::context_entities::BarSection OCXContext::LookupBarSection(
    LDOM_Element const &element) const {
  if (auto res = LDOM2BarSection.find(element); res != LDOM2BarSection.end()) {
    return res->second;
  }
  OCX_ERROR("No Shape found for given LDOM_Element")
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
