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

#ifndef OCX_INCLUDE_OCX_INTERNAL_BAR_SECTION_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_BAR_SECTION_READER_H_

#include <LDOM_Element.hxx>

namespace ocx::reader::class_catalogue::x_section_catalogue {

/**
 * Read the XSectionCatalogue
 */
void ReadXSectionCatalogue(LDOM_Element const &catalogueN);

namespace {  // anonymous namespace
/**
 * Read BarSections from the XSectionCatalogue
 */
void ReadBarSections(LDOM_Element const &xSectionCatalogueN);

/**
 * Read FlatBar and register it in the context
 */
void ReadFlatBar(LDOM_Element const &barSectionN);

}  // namespace

}  // namespace ocx::reader::class_catalogue::x_section_catalogue

#endif  // OCX_INCLUDE_OCX_INTERNAL_BAR_SECTION_READER_H_
