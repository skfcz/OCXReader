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

#include "shipxml/internal/shipxml-properties.h"

#include <string>
#include <vector>

#include "shipxml/internal/shipxml-cartesian-point.h"
#include "shipxml/internal/shipxml-key-value.h"

namespace shipxml {

KeyValue Properties::Add(std::string const &key, std::string const &value) {
  return m_values.emplace_back(key, value);
}

KeyValue Properties::Add(std::string const &key, double value) {
  return Add(key, std::to_string(value));
}

KeyValue Properties::Add(std::string const &key, int value) {
  return Add(key, std::to_string(value));
}

//-----------------------------------------------------------------------------

std::vector<KeyValue> Properties::GetValues() const { return m_values; }

//-----------------------------------------------------------------------------

void Properties::SetWeight(double const &weight) { m_weight = weight; }

double Properties::GetWeight() const { return m_weight; }

//-----------------------------------------------------------------------------

void Properties::SetArea(double const &area) { m_area = area; }

double Properties::GetArea() const { return m_area; }

//-----------------------------------------------------------------------------

void Properties::SetCog(std::string_view mCog) { m_cog = mCog; }

void Properties::SetCog(CartesianPoint const &cp) { SetCog(cp.ToString()); }

std::string Properties::GetCog() const { return m_cog; }

//-----------------------------------------------------------------------------

void Properties::SetBbox0(std::string_view mBbox0) { m_bbox0 = mBbox0; }

void Properties::SetBbox0(CartesianPoint const &cp) { SetBbox0(cp.ToString()); }

std::string Properties::GetBbox0() const { return m_bbox0; }

//-----------------------------------------------------------------------------

void Properties::SetBbox1(std::string_view mBbox1) { m_bbox1 = mBbox1; }

void Properties::SetBbox1(CartesianPoint const &cp) { SetBbox1(cp.ToString()); }

std::string Properties::GetBbox1() const { return m_bbox1; }

//-----------------------------------------------------------------------------

}  // namespace shipxml
