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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_COORDINATE_SYSTEM_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_COORDINATE_SYSTEM_READER_H_

#include <LDOM_Element.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <memory>
#include <utility>

#include "ocx/ocx-context.h"

namespace ocx::vessel::coordinate_system {

void ReadCoordinateSystem(LDOM_Element const &vesselN);

namespace {  // anonymous namespace

TopoDS_Shape ReadRefPlane(LDOM_Element const &refPlanesN,
                          Quantity_Color const &color);

}  // namespace

}  // namespace ocx::vessel::coordinate_system

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_COORDINATE_SYSTEM_READER_H_
