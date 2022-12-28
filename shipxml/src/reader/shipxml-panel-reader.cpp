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

#include <shipxml/internal/shipxml-helper.h>
#include <shipxml/internal/shipxml-curve-reader.h>

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
        Panel panel = this->ReadPanel(aElement);
        m_sst->GetStructure()->AddPanel(panel);
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  std::cout << "transferred #" << m_sst->GetStructure()->GetPanels().size() << " panels" << std::endl;

}

//-----------------------------------------------------------------------------

shipxml::Panel PanelReader::ReadPanel(LDOM_Element const &panelN) {

  OCX_INFO("ReadPanel {}", panelN.getAttribute("name").GetString());

  auto meta = ocx::helper::GetOCXMeta(panelN);

  shipxml::Panel panel(meta->name);

  auto attrs = panelN.GetAttributesList();
  for (int i = 0; i < attrs.getLength(); i++) {
    LDOM_Node node = attrs.item(i);
    panel.GetProperties().Add(node.getNodeName().GetString(),
                              node.getNodeValue().GetString());
  }

  auto descN = ocx::helper::GetFirstChild(panelN, "Description");
  if (!descN.isNull()) {
    panel.GetProperties().Add("description", descN.getFirstChild().getNodeValue().GetString());
    panel.SetCategoryDescription(descN.getFirstChild().getNodeValue().GetString());
  } else {
    OCX_INFO("    no Description child found");
  }

  // the support and outer contour
  ReadSupportAndOuterContour(panelN, panel);

  // the outer contour


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
bool PanelReader::ReadSupportAndOuterContour(const LDOM_Element &panelN, Panel &panel) {

  auto meta = ocx::helper::GetOCXMeta(panelN);


  OCX_DEBUG("ReadSupport {} ", panel.GetName() );
  auto support = panel.GetSupport();

  auto unboundedGeometryN =
      ocx::helper::GetFirstChild(panelN, "UnboundedGeometry");
  if (unboundedGeometryN.isNull()) {
    SHIPXML_WARN("No UnboundedGeometry found in Panel id={} guid={}", meta->id,
                 meta->guid)
    return false;
  }


  // UnboundedGeometry is either a shell, a shell reference, or a grid reference
  gp_Dir supportNormal;

  LDOM_Element refN{};
  if (LDOM_Element gridRefN =
          ocx::helper::GetFirstChild(unboundedGeometryN, "GridRef");
      !gridRefN.isNull()) {
    refN = gridRefN;

    panel.SetIsPlanar(true);

    auto guid= refN.getAttribute("ocx:GUIDRef").GetString();
    OCX_DEBUG("    Using GridRef guid={} as UnboundedGeometry", guid)

    panel.GetProperties().Add("support.gridRef.GUID", guid);

    auto refPlaneW = ocx::OCXContext::GetInstance()->LookupRefPlane(guid );
    if (refPlaneW.refPlaneN.isNull()) {
      SHIPXML_WARN("Failed to lookup an RefPlane for guid={}",guid);
      return false;
    }
    auto refPlaneN = refPlaneW.refPlaneN;
    auto refPlaneType = refPlaneW.type;

    OCX_DEBUG("       RefPlane {} id={}, name={}, guid={}",
              refPlaneType,
                  refPlaneN.getAttribute("id").GetString(),
                  refPlaneN.getAttribute("name").GetString(),
              ocx::helper::GetAttrValue(refPlaneN, "GUIDRef"));


    panel.GetProperties().Add("support.gridRef.id", refPlaneN.getAttribute("id").GetString());
    panel.GetProperties().Add("support.gridRef.name", refPlaneN.getAttribute("name").GetString());
    switch (refPlaneType) {
      case ocx::X : panel.GetProperties().Add("support.gridRef.type", "X");
        break;
      case ocx::Y : panel.GetProperties().Add("support.gridRef.type", "Y");
        break;
      case ocx::Z : panel.GetProperties().Add("support.gridRef.type", "Z");
        break;
    }

    support.SetGrid( refPlaneN.getAttribute("id").GetString());
    support.SetCoordinate( refPlaneN.getAttribute("name").GetString());
    support.SetIsPlanar(true);
    support.SetNormal( shipxml::Convert( refPlaneW.normal));
    panel.GetProperties().Add("support.gridRef.normal", shipxml::ToString( support.GetNormal() ).c_str());
    support.SetTP1( shipxml::Convert( refPlaneW.p1));
    support.SetTP2( shipxml::Convert( refPlaneW.p2));
    support.SetTP3( shipxml::Convert( refPlaneW.p3));

    supportNormal = refPlaneW.normal;

    // TODO: GetSupport gibt immer ein neues Objekt ??
    OCX_DEBUG("       Support grid {}, coordinates {}",
              support.GetGrid(), support.GetCoordinate());


  } else if (LDOM_Element surfaceRefN =
                 ocx::helper::GetFirstChild(unboundedGeometryN, "SurfaceRef");
             !surfaceRefN.isNull()) {
    refN = surfaceRefN;
    OCX_DEBUG("Using SurfaceRef guid={} as UnboundedGeometry",
              refN.getAttribute("ocx:GUIDRef").GetString())
  } else {
    OCX_DEBUG(
        "No GridRef or SurfaceRef child node found in element id={} guid={}. "
        "Try reading directly from UnboundedGeometry.",
        meta->id, meta->guid)
  }

  // currently we only support panels with a planar support
  // TODO: check if a support surface in the limits of the boundary is planar
  if ( ! panel.IsPlanar()) {
    OCX_DEBUG("Do not read OuterContour for none planar panels");
    return true;
  }

  auto outerContourN = ocx::helper::GetFirstChild(panelN, "OuterContour");
  if (outerContourN.isNull()) {
    SHIPXML_WARN("No OuterContour found in Panel id={} guid={}", meta->id, meta->guid)
    return false;
  }

  auto curveN= shipxml::ReadCurve( outerContourN, support.GetMajorPlane(), supportNormal);
  panel.SetGeometry( curveN);


  return true;
  // Read from GridRef or SurfaceRef

  /*if (!refN.isNull()) {
    auto refNMeta = ocx::helper::GetOCXMeta(refN);

    TopoDS_Shape surface = ocx::OCXContext::GetInstance()->LookupShape(refN);
    if (surface.IsNull()) {
      OCX_ERROR("Failed to lookup ReferenceSurface guid={}", refNMeta->guid)
      return {};
    }

    // TODO: Both GridRef and SurfaceRef allow for specifying an offset
    // TODO: parameter of the referenced Surface. This is currently ignored.
    // TODO: Maybe refactor into separate GridRef SurfaceRef functions as its
    // TODO: also used in vessel/panel/limited_by/ocx-limited-by.cc
    return surface;*/
  /*

  // Read directly from UnboundedGeometry
  LDOM_Node childN = unboundedGeometryN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)childN;

      if (TopoDS_Shape surface = ocx::surface::ReadSurface(surfaceN);
          !surface.IsNull()) {
        return surface;
      }
    }
    childN = childN.getNextSibling();
  }

  OCX_ERROR(
      "Failed to directly read surface from UnboundedGeometry in "
      "element id={} guid={}",
      meta->id, meta->guid)
  return {};
*/





  }

}  // namespace shipxml
