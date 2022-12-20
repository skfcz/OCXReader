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

#include "shipxml/internal/shipxml-panel-reader.h"

#include <utility>

#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-limit.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel.h"

namespace shipxml {

PanelReader::PanelReader(LDOM_Element const &vesselN,
                         std::shared_ptr<ShipSteelTransfer> sst)
    : m_sst(std::move(sst)), m_ocxVesselEL(vesselN) {}

//-----------------------------------------------------------------------------

void PanelReader::ReadPanels() const {
  LDOM_Node aChildNode = m_ocxVesselEL.getFirstChild();

  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      if (ocx::helper::GetLocalTagName(aElement) == "Panel") {
        Panel panel = ReadPanel(aElement);
        m_sst->GetStructure()->AddPanel(panel);
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
}

//-----------------------------------------------------------------------------

shipxml::Panel PanelReader::ReadPanel(LDOM_Element const &panelN) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  shipxml::Panel panel(meta->name);

  auto attrs = panelN.GetAttributesList();
  for (int i = 0; i < attrs.getLength(); i++) {
    LDOM_Node node = attrs.item(i);
    panel.GetProperties().Add(node.getNodeName().GetString(),
                              node.getNodeValue().GetString());
  }
  auto desc = ocx::helper::GetFirstChild(panelN, "Description");
  if (!desc.isNull()) {
    panel.GetProperties().Add("description", desc.getNodeValue().GetString());
  }

  // the support
  auto unboundedGeometryN =
      ocx::helper::GetFirstChild(panelN, "UnboundedGeometry");
  if (!unboundedGeometryN.isNull()) {
    // TODO: Get from OCX context
    // ocx::OCXContext::GetInstance()->LookupShape(SomeElement);
  } else {
    SHIPXML_WARN("No UnboundedGeometry found in Panel id={} guid={}", meta->id,
                 meta->guid)
  }

  // the outer contour
  auto outerContourN = ocx::helper::GetFirstChild(panelN, "OuterContour");
  if (!outerContourN.isNull()) {
    // TODO: Get from OCX context
  } else {
    SHIPXML_WARN("No OuterContour found in Panel id={} guid={}", meta->id,
                 meta->guid)
  }

  // the limits
  auto limitedByN = ocx::helper::GetFirstChild(panelN, "LimitedBy");
  if (!limitedByN.isNull()) {
    ReadLimits(limitedByN, panel);
  } else {
    SHIPXML_WARN("No LimitedBy found in Panel id={} guid={}", meta->id,
                 meta->guid)
  }

  return panel;
}

//-----------------------------------------------------------------------------

void PanelReader::ReadLimits(LDOM_Element const &limitedByN, Panel &panel) {
  std::list<shipxml::Limit> limits;

  LDOM_Node aChildN = limitedByN.getFirstChild();
  while (aChildN != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildN;
      auto meta = ocx::helper::GetOCXMeta(aElement);

      if (ocx::helper::GetLocalTagName(aElement) == "FreeEdgeCurve3D") {
        shipxml::Limit limit(meta->id);
        limit.SetFeature(meta->name);
        limit.SetLimitType(CURVE);

        limits.emplace_back(limit);
        // TODO: Get limit curve and write to an limit

      } else if ((ocx::helper::GetLocalTagName(aElement) == "OcxItemPtr") ||
                 ocx::helper::GetLocalTagName(aElement) == "GridRef") {
        shipxml::Limit limit(meta->localRef);
        limit.SetFeature(meta->guid);
        if (meta->refType == "GridRef") {
          limit.SetLimitType(PLANE);
        } else if (meta->refType == "Panel") {
          limit.SetLimitType(PLANE);
        } else {
          SHIPXML_ERROR("Found unknown refType={}", meta->refType)
        }

        limits.emplace_back(limit);
        // TODO: Get limit curve and write to an limit

      } else {
        SHIPXML_ERROR("Found unknown Limit type={}", meta->name)
      }
    }
    aChildN = aChildN.getNextSibling();
  }
  panel.SetLimits(limits);
}

}  // namespace shipxml
