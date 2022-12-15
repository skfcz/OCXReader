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

#include "shipxml/internal/shipxml-panel.h"

#include <string>

#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-extrusion.h"
#include "shipxml/internal/shipxml-limit.h"
#include "shipxml/internal/shipxml-support.h"

namespace shipxml {

Panel::Panel(std::string const& name)
    : EntityWithProperties(name), m_geometry(AMSystem::XY) {}

//-----------------------------------------------------------------------------

void Panel::SetBlockName(std::string_view blockName) {
  m_blockName = blockName;
}

std::string Panel::GetBlockName() const { return m_blockName; }

//-----------------------------------------------------------------------------

void Panel::SetCategory(std::string_view category) { m_category = category; }

std::string Panel::GetCategory() const { return m_category; }

//-----------------------------------------------------------------------------

void Panel::SetCategoryDescription(std::string_view categoryDescription) {
  m_categoryDescription = categoryDescription;
}

std::string Panel::GetCategoryDescription() const {
  return m_categoryDescription;
}

//-----------------------------------------------------------------------------

void Panel::SetIsPlanar(bool isPlanar) { m_planar = isPlanar; }

bool Panel::IsPlanar() const { return m_planar; }

//-----------------------------------------------------------------------------

void Panel::SetIsPillar(bool isPillar) { m_pillar = isPillar; }

bool Panel::IsPillar() const { return m_pillar; }

//-----------------------------------------------------------------------------

void Panel::SetOwner(std::string_view owner) { m_owner = owner; }

std::string Panel::GetOwner() const { return m_owner; }

//-----------------------------------------------------------------------------

void Panel::SetDefaultMaterial(std::string_view defaultMaterial) {
  m_defaultMaterial = defaultMaterial;
}

std::string Panel::GetDefaultMaterial() const { return m_defaultMaterial; }

//-----------------------------------------------------------------------------

void Panel::SetTightness(std::string_view tightness) {
  m_tightness = tightness;
}

std::string Panel::GetTightness() const { return m_tightness; }

//-----------------------------------------------------------------------------

shipxml::Extrusion Panel::GetExtrusion() const { return m_extrusion; }

//-----------------------------------------------------------------------------

shipxml::Support Panel::GetSupport() const { return m_support; }

//-----------------------------------------------------------------------------

void Panel::SetLimits(std::list<shipxml::Limit> const& limits) {
  m_limits = limits;
}

std::list<shipxml::Limit> Panel::GetLimits() const { return m_limits; }

//-----------------------------------------------------------------------------

void Panel::SetGeometry(shipxml::AMCurve const& geometry) {
  m_geometry = geometry;
}

shipxml::AMCurve Panel::GetGeometry() const { return m_geometry; }

}  // namespace shipxml
