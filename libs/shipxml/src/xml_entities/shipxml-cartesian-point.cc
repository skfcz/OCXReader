/***************************************************************************
 *   Created on: 01 Dec 2022                                               *
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

#include "shipxml/internal/shipxml-cartesian-point.h"

namespace shipxml {

CartesianPoint::CartesianPoint() = default;

//-----------------------------------------------------------------------------

CartesianPoint::CartesianPoint(double x, double y, double z)
    : m_x(x), m_y(y), m_z(z) {}

//-----------------------------------------------------------------------------

void CartesianPoint::SetX(double value) { m_x = value; }

double CartesianPoint::GetX() const { return m_x; }

//-----------------------------------------------------------------------------

void CartesianPoint::SetY(double value) { m_y = value; }

double CartesianPoint::GetY() const { return m_y; }

//-----------------------------------------------------------------------------

void CartesianPoint::SetZ(double value) { m_z = value; }

double CartesianPoint::GetZ() const { return m_z; }

}  // namespace shipxml
