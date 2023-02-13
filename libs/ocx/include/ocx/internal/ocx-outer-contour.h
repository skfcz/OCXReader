/***************************************************************************
 *   Created on: 06 Dec 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_OUTER_CONTOUR_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_OUTER_CONTOUR_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Wire.hxx>

namespace ocx::reader::shared::outer_contour {

TopoDS_Wire ReadOuterContour(LDOM_Element const& elementN);

}  // namespace ocx::reader::shared::outer_contour

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_OUTER_CONTOUR_H_
