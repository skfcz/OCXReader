/***************************************************************************
 *   Created on: 07 Dec 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_COMPOSED_OF_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_COMPOSED_OF_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

namespace ocx::reader::vessel::panel::composed_of {

[[nodiscard]] TopoDS_Shape ReadComposedOf(LDOM_Element const &panelN,
                                          bool withLimitedBy = false);
namespace {  // anonymous namespace

[[nodiscard]] TopoDS_Shape ReadPlate(LDOM_Element const &panelN,
                                     LDOM_Element const &plateN,
                                     bool withLimitedBy = false);

[[nodiscard]] TopoDS_Shape ReadBracket(LDOM_Element const &bracketN);

}  // namespace

}  // namespace ocx::reader::vessel::panel::composed_of

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_COMPOSED_OF_H_
