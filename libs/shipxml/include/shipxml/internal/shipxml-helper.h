/***************************************************************************
 *   Created on: 13 Dec 2022                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_HELPER_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_HELPER_H_

#include <gp_Pnt.hxx>
#include <vector>

namespace  shipxml {

[[nodiscard]] std::vector<shipxml::ArcSegment> ReadCurve(
    LDOM_Element const &curveRootN);

[[nodiscard]] shipxml::CartesianPoint Convert( gp_Pnt point);

[[nodiscard]] shipxml::Vector Convert( gp_Dir d);

[[nodiscard]] std::string ToString( shipxml::CartesianPoint p);

[[nodiscard]] std::string ToString( shipxml::Vector v);

}

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_HELPER_H_
