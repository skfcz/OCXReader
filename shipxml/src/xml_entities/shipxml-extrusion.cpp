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

#include "shipxml/internal/shipxml-extrusion.h"

#include <string>

namespace shipxml {

Extrusion::Extrusion() : EntityWithProperties(std::string_view()) {}

//-----------------------------------------------------------------------------

void Extrusion::SetThickness(double thickness) { m_thickness = thickness; }

double Extrusion::GetThickness() const { return m_thickness; }

//-----------------------------------------------------------------------------

void Extrusion::SetOffset(double offset) { m_offset = offset; }

double Extrusion::GetOffset() const { return m_offset; }

//-----------------------------------------------------------------------------

void Extrusion::SetOrientation(shipxml::Orientation orientation) {
  m_orientation = orientation;
}

shipxml::Orientation Extrusion::GetOrientation() const { return m_orientation; }

//-----------------------------------------------------------------------------

}  // namespace shipxml
