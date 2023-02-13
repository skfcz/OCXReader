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

#include <BRepAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <chrono>
#include <magic_enum.hpp>
#include <random>
#include <utility>

#include "occutils/occutils-shape-components.h"
#include "occutils/occutils-surface.h"
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
  if (auto descN = ocx::helper::GetFirstChild(panelN, "Description");
      !descN.isNull()) {
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
  if (auto limitedByN = ocx::helper::GetFirstChild(panelN, "LimitedBy");
      !limitedByN.isNull()) {
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

  SHIPXML_INFO("ReadSupportAndOuterContour {}", panel.GetName());

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

    auto gridMeta = ocx::helper::GetOCXMeta(refN);
    SHIPXML_DEBUG("    Using GridRef guid={} as UnboundedGeometry",
                  gridMeta->guid)
    panel.GetProperties().Add("support.gridRef.GUID", meta->guid);

    auto refPlaneW =
        ocx::OCXContext::GetInstance()->LookupRefPlane(gridMeta->guid);
    if (refPlaneW.m_refPlaneN.isNull()) {
      SHIPXML_WARN("Failed to lookup an RefPlane for guid={}", meta->guid)
      return;
    }

    auto refPlaneType = refPlaneW.m_type;
    auto refPlaneMeta = ocx::helper::GetOCXMeta(refPlaneW.m_refPlaneN);

    SHIPXML_DEBUG("       RefPlane {} id={}, name={}, guid={}",
                  magic_enum::enum_name(refPlaneType), refPlaneMeta->id,
                  refPlaneMeta->name, refPlaneMeta->guid)

    panel.GetProperties().Add("support.gridRef.id", refPlaneMeta->id);
    panel.GetProperties().Add("support.gridRef.name", refPlaneMeta->name);
    switch (refPlaneType) {
      case ocx::context_entities::RefPlaneType::X:
        panel.GetProperties().Add("support.gridRef.type", "X");
        support.SetLocationType(LocationType::X);
        support.SetMajorPlane(MajorPlane::X);
        break;
      case ocx::context_entities::RefPlaneType::Y:
        panel.GetProperties().Add("support.gridRef.type", "Y");
        support.SetLocationType(LocationType::Y);
        support.SetMajorPlane(MajorPlane::Y);
        break;
      case ocx::context_entities::RefPlaneType::Z:
        panel.GetProperties().Add("support.gridRef.type", "Z");
        support.SetLocationType(LocationType::Z);
        support.SetMajorPlane(MajorPlane::Z);
        break;
      default:
        SHIPXML_ERROR("Undefined RefPlaneType={}",
                      magic_enum::enum_name(refPlaneType))
    }

    support.SetIsPlanar(true);
    support.SetGrid(refPlaneMeta->id);
    support.SetCoordinate(refPlaneMeta->name);
    support.SetNormal(refPlaneW.m_normal);
    support.SetTP1(refPlaneW.m_p1);
    support.SetTP2(refPlaneW.m_p2);
    support.SetTP3(refPlaneW.m_p3);

    panel.GetProperties().Add("support.gridRef.normal",
                              support.GetNormal().ToString());

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

    // TODO: check if a support surface in the limits of the boundary is planar
    // Proposed solution get the panel shape from context and check its face
    // geometry if it is planar
    TopoDS_Shape panelShape =
        ocx::OCXContext::GetInstance()->LookupShape(panelN);
    if (!panelShape.IsNull()) {
      if (std::vector<TopoDS_Face> faces =
              OCCUtils::ShapeComponents::AllFacesWithin(panelShape);
          faces.size() == 1) {
        GeomAdaptor_Surface adaptor = OCCUtils::Surface::FromFace(faces.at(0));
        if (adaptor.GetType() == GeomAbs_Plane) {
          panel.SetIsPlanar(true);
          support.SetIsPlanar(true);

          Handle_Geom_Plane plane =
              Handle_Geom_Plane::DownCast(adaptor.Surface());
          gp_Pln pln = plane->Pln();
          gp_Dir normal = pln.Axis().Direction();

          // Set normal dir
          support.SetNormal(Vector(normal));

          // Get major plane and location type
          if (abs(normal.Z()) < Precision::Confusion()) {
            // The face is near XY plane
            support.SetMajorPlane(MajorPlane::Z);
            support.SetLocationType(LocationType::Z);
          } else if (abs(normal.Y()) < Precision::Confusion()) {
            // The face is near XZ plane
            support.SetMajorPlane(MajorPlane::Y);
            support.SetLocationType(LocationType::Y);
          } else if (abs(normal.X()) < Precision::Confusion()) {
            // The face is near YZ plane
            support.SetMajorPlane(MajorPlane::X);
            support.SetLocationType(LocationType::X);
          } else {
            // The face is not near any of the major planes
            support.SetMajorPlane(MajorPlane::UNDEFINED);
            support.SetLocationType(LocationType::TP);
          }

          // Handle tp1, tp2, tp3
          BRepAdaptor_Surface brepAdaptor(faces.at(0));
          double uMin = brepAdaptor.FirstUParameter();
          double uMax = brepAdaptor.LastUParameter();
          double vMin = brepAdaptor.FirstVParameter();
          double vMax = brepAdaptor.LastVParameter();

          // Get 3 random points on the surface
          std::mt19937 gen(
              std::chrono::system_clock::now().time_since_epoch().count());
          std::uniform_real_distribution distU(uMin, uMax);
          std::uniform_real_distribution distV(vMin, vMax);

          for (int i = 0; i < 3; i++) {
            double u = distU(gen);
            double v = distV(gen);

            if (i == 0)
              support.SetTP1(brepAdaptor.Value(u, v));
            else if (i == 1)
              support.SetTP2(brepAdaptor.Value(u, v));
            else if (i == 2)
              support.SetTP3(brepAdaptor.Value(u, v));
          }
        } else {
          support.SetIsPlanar(false);
          panel.SetIsPlanar(false);
        }

      } else {
        SHIPXML_WARN("Got {} faces from panel shape. Expected 1", faces.size())
        support.SetIsPlanar(false);
        panel.SetIsPlanar(false);
      }
    } else {
      SHIPXML_WARN("No faces found in panel shape")
      support.SetIsPlanar(false);
      panel.SetIsPlanar(false);
    }
  }
  panel.SetSupport(support);

  // TODO: Currently we only support panels with a planar support
  if (!panel.GetSupport().IsPlanar()) {
    SHIPXML_WARN("Do not read OuterContour for none planar panel {}",
                 panel.GetName())
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

      if (TopoDS_Shape surface =
  ocx::reader::shared::surface::ReadSurface(surfaceN); !surface.IsNull()) {
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

void PanelReader::ReadComposedOf(LDOM_Element const &panelN, Panel &panel) {
  PlateReader plateReader;
  plateReader.ReadPlates(panelN, panel);
}

}  // namespace shipxml
