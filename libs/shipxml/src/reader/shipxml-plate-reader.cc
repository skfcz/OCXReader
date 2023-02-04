/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
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

#include <shipxml/internal/shipxml-curve-reader.h>
#include <shipxml/internal/shipxml-plate-reader.h>

#include <LDOM_Element.hxx>
#include <LDOM_Node.hxx>

#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel.h"
#include "shipxml/internal/shipxml-plate.h"

namespace shipxml {

void PlateReader::ReadPlates(LDOM_Element const &ocxPanelN,
                             Panel &panel) const {
  SHIPXML_INFO("ReadPlates {}", panel.GetName())

  auto composedOfEL = ocx::helper::GetFirstChild(ocxPanelN, "ComposedOf");
  if (composedOfEL.isNull()) {
    SHIPXML_INFO("    no ComposedOf found in {}", panel.GetName())
    return;
  }

  LDOM_Node aChildNode = composedOfEL.getFirstChild();

  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      if (ocx::helper::GetLocalTagName(aElement) == "Plate") {
        panel.AddPlate(ReadPlate(aElement, panel));
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  SHIPXML_INFO("    read #{} plates", panel.GetPlates().size())
}

//-----------------------------------------------------------------------------

Plate PlateReader::ReadPlate(const LDOM_Element &plateN, Panel const &panel) {
  auto meta = ocx::helper::GetOCXMeta(plateN);

  auto plate = Plate(meta->id);
  auto properties = plate.GetProperties();
  properties.Add("id", meta->id);
  properties.Add("guid", meta->guid);

  if (LDOM_Element ppN =
          ocx::helper::GetFirstChild(plateN, "PhysicalProperties");
      ppN.isNull()) {
    SHIPXML_ERROR("No PhysicalProperties in Plate id={} guid={}", meta->id,
                  meta->guid)
  } else {
    if (LDOM_Element cogN = ocx::helper::GetFirstChild(ppN, "CenterOfGravity");
        cogN.isNull()) {
      SHIPXML_ERROR("No CenterOfGravity found in Plate id={} guid={}", meta->id,
                    meta->guid)
    } else {
      gp_Pnt cog = ocx::helper::ReadPoint(cogN);
      properties.SetCog(cog);
    }

    LDOM_Element wghtN = ocx::helper::GetFirstChild(ppN, "DryWeight");
    if (wghtN.isNull()) {
      SHIPXML_ERROR("No DryWeight found in Plate id={} guid={}", meta->id,
                    meta->guid)
    } else {
      double weight = ocx::helper::ReadDimension(wghtN);
      properties.SetWeight(weight);
    }
  }

  // TODO: Plate Material

  // currently we only support plates on planar panels
  if (!panel.GetSupport().IsPlanar()) {
    SHIPXML_DEBUG("Do not read OuterContour for none planar panels")
  } else {
    auto outerContourN = ocx::helper::GetFirstChild(plateN, "OuterContour");
    if (outerContourN.isNull()) {
      SHIPXML_WARN("No OuterContour found in Panel id={} guid={}", meta->id,
                   meta->guid)
    } else {
      auto curveN = ReadCurve(outerContourN, panel.GetSupport().GetMajorPlane(),
                              panel.GetSupport().GetNormal().ToDir());
      plate.SetGeometry(curveN);
    }
  }

  return plate;
}

}  // namespace shipxml
