/***************************************************************************
 *   Created on: 28 Nov 2022                                               *
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

#include "ocx/internal/ocx-x-section-catalogue.h"

#include <memory>

#include "LDOM_Element.hxx"
#include "ocx/internal/ocx-utils.h"
#include "ocx/ocx-helper.h"

namespace ocx::reader::class_catalogue::x_section_catalogue {

void ReadXSectionCatalogue(LDOM_Element const &catalogueN) {
  LDOM_Element xSectionCatalogueN =
      ocx::helper::GetFirstChild(catalogueN, "XSectionCatalogue");
  if (xSectionCatalogueN.isNull()) {
    OCX_ERROR("No XSectionCatalogue child node found.")
    return;
  }

  // TODO: Read BarSection
}

//-----------------------------------------------------------------------------

namespace {

void ReadBarSections(LDOM_Element const &xSectionCatalogueN) {
  LDOM_Element barSectionN =
      ocx::helper::GetFirstChild(xSectionCatalogueN, "BarSection");
  if (barSectionN.isNull()) {
    OCX_ERROR("No BarSection child node found in XSectionCatalogue")
    return;
  }

  LDOM_Node childN = barSectionN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element elementN = (LDOM_Element &)childN;

      auto barSectionMeta = ocx::helper::GetOCXMeta(elementN);

      std::string barSectionType = ocx::helper::GetChildTagName(elementN);
      if (barSectionType == "FlatBar") {
        ReadFlatBar(elementN);
      } else {
        OCX_ERROR(
            "Found unsupported BarSection type {} in ReadBarSection with "
            "BarSection id={} guid={}",
            barSectionType, barSectionMeta->id, barSectionMeta->guid)
      }
    }
    childN = childN.getNextSibling();
  }
}

//-----------------------------------------------------------------------------

void ReadFlatBar(LDOM_Element const &barSectionN) {
  auto meta = ocx::helper::GetOCXMeta(barSectionN);

  // Read height and width
  LDOM_Element heightElement =
      ocx::helper::GetFirstChild(barSectionN, "Height");
  if (heightElement.isNull()) {
    OCX_ERROR(
        "No Height child node found in ReadFlatBar with BarSection id={} "
        "guid={}",
        meta->id, meta->guid)
    return;
  }

  LDOM_Element widthElement = ocx::helper::GetFirstChild(barSectionN, "Width");
  if (widthElement.isNull()) {
    OCX_ERROR(
        "No Width child node found in ReadFlatBar with BarSection id={} "
        "guid={}",
        meta->id, meta->guid)
    return;
  }

  double height = ocx::helper::ReadDimension(heightElement);
  double width = ocx::helper::ReadDimension(widthElement);

  // BarSection flatBar = BarSection::FlatBar(height, width);

  // Register FlatBar
  // m_ctx->RegisterBarSection(barSectionInfo->guid, height, width);
}

}  // namespace

}  // namespace ocx::reader::class_catalogue::x_section_catalogue
