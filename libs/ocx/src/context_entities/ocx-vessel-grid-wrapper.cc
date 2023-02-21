/***************************************************************************
 *   Created on: 16 Feb 2023                                               *
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

#include "ocx/internal/ocx-vessel-grid-wrapper.h"

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

namespace ocx::context_entities {

SpacingGroup::SpacingGroup(Standard_Integer const &firstGridNumber,
                           Standard_Integer const &count,
                           Standard_Real const &spacing,
                           Standard_Real const &gridPosition)
    : m_firstGridNumber(firstGridNumber),
      m_count(count),
      m_spacing(spacing),
      m_gridPosition(gridPosition) {}

//-----------------------------------------------------------------------------

VesselGridWrapper::VesselGridWrapper(VesselGridType const &type)
    : m_type(type) {}

//-----------------------------------------------------------------------------

void VesselGridWrapper::SetVesselGridType(VesselGridType const &type) {
  m_type = type;
}

VesselGridType VesselGridWrapper::GetVesselGridType() const { return m_type; }

//-----------------------------------------------------------------------------

void VesselGridWrapper::SetDistanceToAp(Standard_Real const &distanceToAp) {
  m_distanceToAp = distanceToAp;
}

std::optional<Standard_Real> VesselGridWrapper::GetDistanceToAp() const {
  return m_distanceToAp;
}

//-----------------------------------------------------------------------------

void VesselGridWrapper::AddSpacingGroup(SpacingGroup const &spacingGroup) {
  m_spacingGroups.push_back(spacingGroup);
}

std::vector<SpacingGroup> VesselGridWrapper::GetSpacingGroups() const {
  return m_spacingGroups;
}

//-----------------------------------------------------------------------------

void VesselGridWrapper::SetIsReversed(bool const &isReversed) {
  m_isReversed = isReversed;
}

bool VesselGridWrapper::GetIsReversed() const { return m_isReversed; }

//-----------------------------------------------------------------------------

void VesselGridWrapper::SetIsMainSystem(bool const &isMainSystem) {
  m_isMainSystem = isMainSystem;
}

bool VesselGridWrapper::GetIsMainSystem() const { return m_isMainSystem; }

}  // namespace ocx::context_entities
