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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_LIMITED_BY_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_LIMITED_BY_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>

#include "ocx/internal/ocx-utils.h"
#include "ocx/ocx-context.h"

namespace ocx::reader::shared::limited_by {

/**
 * @brief Read LimitedBy by calculating the intersection between the passed
 * surface and surface referenced by the OcxItemPtr
 *
 * @param panelN the panel to limit
 * @return the limitedBy assembly
 */
[[nodiscard]] TopoDS_Shape ReadLimitedBy(LDOM_Element const &panelN);

namespace {  // anonymous namespace

/**
 * Read OcxItemPtr from LimitedBy
 *
 * @return the referenced shape restricted by the given bounding box
 */
[[nodiscard]] TopoDS_Shape ReadOcxItemPtr(LDOM_Element const &panelN,
                                          LDOM_Element const &ocxItemPtrN);

[[nodiscard]] TopoDS_Shape ReadFreeEdgeCurve3D(LDOM_Element const &curveN);

[[nodiscard]] TopoDS_Shape ReadGridRef(LDOM_Element const &panelN,
                                       LDOM_Element const &gridRefN);

}  // namespace

}  // namespace ocx::reader::shared::limited_by

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_LIMITED_BY_READER_H_
