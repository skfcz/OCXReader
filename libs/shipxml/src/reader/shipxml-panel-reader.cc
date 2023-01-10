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

#include <magic_enum.hpp>
#include <utility>

#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-limit.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel.h"
#include "shipxml/internal/shipxml-plate-reader.h"

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

Panel PanelReader::ReadPanel(LDOM_Element const &panelN) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  Panel panel(meta->name);

  auto attrs = panelN.GetAttributesList();
  for (int i = 0; i < attrs.getLength(); i++) {
    LDOM_Node node = attrs.item(i);
    panel.GetProperties().Add(node.getNodeName().GetString(),
                              node.getNodeValue().GetString());
  }
  auto descN = ocx::helper::GetFirstChild(panelN, "Description");
  if (!descN.isNull()) {
    panel.GetProperties().Add("description",
                              descN.getFirstChild().getNodeValue().GetString());
    panel.SetCategoryDescription(
        descN.getFirstChild().getNodeValue().GetString());
  } else {
    SHIPXML_INFO("    no Description child found")
  }

  // the support and outer contour
  ReadSupportAndOuterContour(panelN, panel);

  // the limits
  auto limitedByN = ocx::helper::GetFirstChild(panelN, "LimitedBy");
  if (!limitedByN.isNull()) {
    ReadLimits(limitedByN, panel);
  } else {
    SHIPXML_WARN("No LimitedBy found in Panel id={} guid={}", meta->id,
                 meta->guid)
  }

  // the plates
  ReadComposedOf(panelN, panel);

  return panel;
}

//-----------------------------------------------------------------------------

void PanelReader::ReadLimits(LDOM_Element const &limitedByN, Panel &panel) {
  std::list<Limit> limits;

  LDOM_Node aChildN = limitedByN.getFirstChild();
  while (aChildN != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildN;
      auto meta = ocx::helper::GetOCXMeta(aElement);

      if (ocx::helper::GetLocalTagName(aElement) == "FreeEdgeCurve3D") {
        Limit limit(meta->id);
        limit.SetFeature(meta->name);
        limit.SetLimitType(LimitType::CURVE);

        limits.emplace_back(limit);
        // TODO: Get limit curve and write to an limit

      } else if ((ocx::helper::GetLocalTagName(aElement) == "OcxItemPtr") ||
                 ocx::helper::GetLocalTagName(aElement) == "GridRef") {
        Limit limit(meta->localRef);
        limit.SetFeature(meta->guid);
        if (meta->refType == "GridRef") {
          limit.SetLimitType(LimitType::PLANE);
        } else if (meta->refType == "Panel") {
          limit.SetLimitType(LimitType::PLANE);
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

//-----------------------------------------------------------------------------

void PanelReader::ReadSupportAndOuterContour(const LDOM_Element &panelN,
                                             Panel &panel) {
  auto meta = ocx::helper::GetOCXMeta(panelN);

  SHIPXML_DEBUG("ReadSupport {} ", panel.GetName())

  auto unboundedGeometryN =
      ocx::helper::GetFirstChild(panelN, "UnboundedGeometry");
  if (unboundedGeometryN.isNull()) {
    SHIPXML_WARN("No UnboundedGeometry found in Panel id={} guid={}", meta->id,
                 meta->guid)
    return;
  }

  // Create support object
  Support support;

  // UnboundedGeometry is either a shell, a shell reference, or a grid reference
  LDOM_Element refN{};
  if (LDOM_Element gridRefN =
          ocx::helper::GetFirstChild(unboundedGeometryN, "GridRef");
      !gridRefN.isNull()) {
    refN = gridRefN;

    SHIPXML_DEBUG("    Using GridRef guid={} as UnboundedGeometry", meta->guid)

    panel.GetProperties().Add("support.gridRef.GUID", meta->guid);

    auto refPlaneW = ocx::OCXContext::GetInstance()->LookupRefPlane(meta->guid);
    if (refPlaneW.refPlaneN.isNull()) {
      SHIPXML_WARN("Failed to lookup an RefPlane for guid={}", meta->guid)
      return;
    }

    auto refPlaneType = refPlaneW.type;
    auto refPlaneMeta = ocx::helper::GetOCXMeta(refPlaneW.refPlaneN);

    SHIPXML_DEBUG("       RefPlane {} id={}, name={}, guid={}",
                  magic_enum::enum_name(refPlaneType), refPlaneMeta->id,
                  refPlaneMeta->name, refPlaneMeta->guid)

    panel.GetProperties().Add("support.gridRef.id", refPlaneMeta->id);
    panel.GetProperties().Add("support.gridRef.name", refPlaneMeta->name);
    switch (refPlaneType) {
      case ocx::RefPlaneType::X:
        panel.GetProperties().Add("support.gridRef.type", "X");
        break;
      case ocx::RefPlaneType::Y:
        panel.GetProperties().Add("support.gridRef.type", "Y");
        break;
      case ocx::RefPlaneType::Z:
        panel.GetProperties().Add("support.gridRef.type", "Z");
        break;
      default:
        SHIPXML_ERROR("Undefined RefPlaneType={}",
                      magic_enum::enum_name(refPlaneType))
    }

    support.SetGrid(refPlaneMeta->id);
    support.SetCoordinate(refPlaneMeta->name);
    support.SetIsPlanar(true);
    support.SetNormal(refPlaneW.normal);
    support.SetTP1(CartesianPoint(refPlaneW.p1));
    support.SetTP2(CartesianPoint(refPlaneW.p2));
    support.SetTP3(CartesianPoint(refPlaneW.p3));

    panel.GetProperties().Add("support.gridRef.normal",
                              support.GetNormal().ToString().c_str());

    SHIPXML_DEBUG("       Support grid {}, coordinates {}", support.GetGrid(),
                  support.GetCoordinate())
  } else if (LDOM_Element surfaceRefN =
                 ocx::helper::GetFirstChild(unboundedGeometryN, "SurfaceRef");
             !surfaceRefN.isNull()) {
    refN = surfaceRefN;
    SHIPXML_DEBUG("Using SurfaceRef guid={} as UnboundedGeometry",
                  refN.getAttribute("ocx:GUIDRef").GetString())
  } else {
    SHIPXML_DEBUG(
        "No GridRef or SurfaceRef child node found in element id={} guid={}. "
        "Try reading directly from UnboundedGeometry.",
        meta->id, meta->guid)
  }
  panel.SetSupport(support);

  // currently we only support panels with a planar support
  // TODO: check if a support surface in the limits of the boundary is planar
  if (!panel.IsPlanar()) {
    SHIPXML_DEBUG("Do not read OuterContour for none planar panels")
    return;
  }

  auto outerContourN = ocx::helper::GetFirstChild(panelN, "OuterContour");
  if (outerContourN.isNull()) {
    SHIPXML_WARN("No OuterContour found in Panel id={} guid={}", meta->id,
                 meta->guid)
    return;
  }

  auto curveN = shipxml::ReadCurve(outerContourN, support.GetMajorPlane(),
                                   support.GetNormal().ToDir());
  panel.SetGeometry(curveN);

  return;
  // Read from GridRef or SurfaceRef

  /*if (!refN.isNull()) {
    auto refNMeta = ocx::helper::GetOCXMeta(refN);

    TopoDS_Shape surface = ocx::OCXContext::GetInstance()->LookupShape(refN);
    if (surface.IsNull()) {
      SHIPXML_ERROR("Failed to lookup ReferenceSurface guid={}", refNMeta->guid)
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

  SHIPXML_ERROR(
      "Failed to directly read surface from UnboundedGeometry in "
      "element id={} guid={}",
      meta->id, meta->guid)
  return {};
*/
}

//-----------------------------------------------------------------------------

void PanelReader::ReadComposedOf(LDOM_Element const &panelN,
                                 Panel const &panel) {
  PlateReader plateReader;
  plateReader.ReadPlates(panelN, panel);
}

}  // namespace shipxml
