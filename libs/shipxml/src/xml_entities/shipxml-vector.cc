/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
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

#include "shipxml/internal/shipxml-vector.h"

#include <gp_Dir.hxx>
#include <string>

namespace shipxml {

Vector::Vector() = default;

//-----------------------------------------------------------------------------

Vector::Vector(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

//-----------------------------------------------------------------------------

Vector::Vector(gp_Dir const& dir) : m_x(dir.X()), m_y(dir.Y()), m_z(dir.Z()) {}

//-----------------------------------------------------------------------------

void Vector::SetX(double value) { m_x = value; }

double Vector::GetX() const { return m_x; }

//-----------------------------------------------------------------------------

void Vector::SetY(double value) { m_y = value; }

double Vector::GetY() const { return m_y; }

//-----------------------------------------------------------------------------

void Vector::SetZ(double value) { m_z = value; }

double Vector::GetZ() const { return m_z; }

//-----------------------------------------------------------------------------

gp_Dir Vector::ToDir() const { return {m_x, m_y, m_z}; }

//-----------------------------------------------------------------------------

std::string Vector::ToString() const {
  return "(" + std::to_string(m_x) + ", " + std::to_string(m_y) + ", " +
         std::to_string(m_z) + ")";
}

}  // namespace shipxml
