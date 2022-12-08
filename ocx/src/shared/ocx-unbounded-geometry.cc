/***************************************************************************
 *   Created on: 29 Nov 2022                                               *
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

#include "ocx/internal/ocx-unbounded-geometry.h"

#include <LDOM_Element.hxx>

#include "ocx/internal/ocx-helper.h"

namespace ocx::shared::unbounded_geometry {

TopoDS_Shape ReadUnboundedGeometry(LDOM_Element const &elementN) {
  std::unique_ptr<ocx::helper::OCXMeta> meta =
      ocx::helper::GetOCXMeta(elementN);

  LDOM_Element unboundedGeometryN =
      ocx::helper::GetFirstChild(elementN, "UnboundedGeometry");
  if (unboundedGeometryN.isNull()) {
    OCX_ERROR("No UnboundedGeometry child node found in element id={} guid={}",
              meta->id, meta->guid);
    return {};
  }

  // UnboundedGeometry is either a shell, a shell reference, or a grid reference
  LDOM_Element refN{};
  if (LDOM_Element gridRefN =
          ocx::helper::GetFirstChild(unboundedGeometryN, "GridRef");
      !gridRefN.isNull()) {
    refN = gridRefN;
    OCX_DEBUG("Using GridRef guid={} as UnboundedGeometry",
              refN.getAttribute("ocx:GUIDRef").GetString());
  } else if (LDOM_Element surfaceRefN =
                 ocx::helper::GetFirstChild(unboundedGeometryN, "SurfaceRef");
             !surfaceRefN.isNull()) {
    refN = surfaceRefN;
    OCX_DEBUG("Using SurfaceRef guid={} as UnboundedGeometry",
              refN.getAttribute("ocx:GUIDRef").GetString());
  } else {
    OCX_DEBUG(
        "No GridRef or SurfaceRef child node found in element id={} guid={}. "
        "Try reading directly from UnboundedGeometry.",
        meta->id, meta->guid);
  }

  // Read from GridRef or SurfaceRef
  if (!refN.isNull()) {
    std::unique_ptr<ocx::helper::OCXMeta> refNMeta =
        ocx::helper::GetOCXMeta(refN);

    ExtendedShape surface = OCXContext::GetInstance()->LookupShape(refN);
    if (surface.m_shape.IsNull()) {
      OCX_ERROR("Failed to lookup ReferenceSurface guid={}", refNMeta->guid);
      return {};
    }

    // TODO: Both GridRef and SurfaceRef allow for specifying an offset
    // TODO: parameter of the referenced Surface. This is currently ignored.
    // TODO: Maybe refactor into separate GridRef SurfaceRef functions as its
    // TODO: also used in vessel/panel/limited_by/ocx-limited-by.cc
    return surface.m_shape;
  }

  // Read directly from UnboundedGeometry
  LDOM_Node aChildNode = unboundedGeometryN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)aChildNode;

      if (TopoDS_Shape surface = ocx::surface::ReadSurface(surfaceN);
          !surface.IsNull()) {
        return surface;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  OCX_ERROR("Failed to read surface from UnboundedGeometry");
  return {};
}

}  // namespace ocx::shared::unbounded_geometry
