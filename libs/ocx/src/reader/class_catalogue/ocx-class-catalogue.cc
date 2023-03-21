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

#include "ocx/internal/ocx-class-catalogue.h"

#include "ocx/internal/ocx-x-section-catalogue.h"
#include "ocx/internal/ocx-hole-catalogue.h"
#include "ocx/ocx-helper.h"

namespace ocx::reader::class_catalogue {

void ReadClassCatalogue() {
  LDOM_Element catalogueN = ocx::helper::GetFirstChild(
      OCXContext::GetInstance()->OCXRoot(), "ClassCatalogue");
  if (catalogueN.isNull()) {
    OCX_ERROR("No ClassCatalogue child node found.")
    return;
  }

  // TODO: Read MaterialCatalogue

  // Read XSectionCatalogue
  ocx::reader::class_catalogue::x_section_catalogue::ReadXSectionCatalogue(
      catalogueN);

  // TODO: Read HoleShapeCatalogue

  ocx::hole_catalogue::ReadHoleCatalogue(catalogueN);
}

}  // namespace ocx::reader::class_catalogue
