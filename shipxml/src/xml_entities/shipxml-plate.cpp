/***************************************************************************
*   Created on: 29.12.22.                                                 *
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
#include "shipxml/internal/shipxml-plate.h"

namespace shipxml {
Plate::Plate(std::string const& name)
    : EntityWithProperties(name), m_geometry(AMSystem::XY) {}
//-----------------------------------------------------------------------------
void Plate::SetCategory(std::string_view category) { m_category = category; }
std::string Plate::GetCategory() const { return m_category; }
//-----------------------------------------------------------------------------
void Plate::SetCategoryDescription(std::string_view categoryDescription) {
  m_categoryDescription = categoryDescription;
}
std::string Plate::GetCategoryDescription() const {
  return m_categoryDescription;
}
//-----------------------------------------------------------------------------
void Plate::SetMaterial(std::string_view defaultMaterial) {
  m_material = defaultMaterial;
}
std::string Plate::GetMaterial() const { return m_material; }

//-----------------------------------------------------------------------------
void Plate::SetThickness(double t) { m_thickness = t; }
double Plate::GetThickness() const { return m_thickness; }
//-----------------------------------------------------------------------------
Orientation Plate::GetOrientation() const { return m_orientation; }
void Plate::SetOrientation(Orientation o) { m_orientation = o; }
//-----------------------------------------------------------------------------
double Plate::GetOffset() const { return m_offset; }
void Plate::SetOffset(double t) { m_offset = t; }

//-----------------------------------------------------------------------------
void Plate::SetGeometry(shipxml::AMCurve const& geometry) {
  m_geometry = geometry;
}

shipxml::AMCurve Plate::GetGeometry() const { return m_geometry; }

}  // namespace shipxml