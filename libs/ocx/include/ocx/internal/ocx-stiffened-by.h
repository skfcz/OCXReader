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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_STIFFENER_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_STIFFENER_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <memory>
#include <string>

namespace ocx::reader::vessel::panel::stiffened_by {

[[nodiscard]] TopoDS_Shape ReadStiffenedBy(LDOM_Element const &panelN,
                                           bool addShape = true);

namespace {  // anonymous namespace

[[nodiscard]] TopoDS_Shape ReadStiffener(LDOM_Element const &stiffenerN,
                                         bool addShape = true);

}  // namespace

}  // namespace ocx::reader::vessel::panel::stiffened_by

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_STIFFENER_READER_H_
