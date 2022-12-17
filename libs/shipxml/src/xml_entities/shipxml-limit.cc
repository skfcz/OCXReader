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

#include "shipxml/internal/shipxml-limit.h"

#include <string>

#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-cartesian-point.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

Limit::Limit(std::string_view name) : NamedEntity(name) {}

//-----------------------------------------------------------------------------

void Limit::SetContour(shipxml::AMCurve const& contour) { m_contour = contour; }

shipxml::AMCurve Limit::GetContour() const { return m_contour; }

//-----------------------------------------------------------------------------

void Limit::SetFeature(std::string_view feature) { m_feature = feature; }

std::string Limit::GetFeature() const { return m_feature; }

//-----------------------------------------------------------------------------

void Limit::SetLimitType(shipxml::LimitType limitType) {
  m_limitType = limitType;
}

shipxml::LimitType Limit::GetLimitType() const { return m_limitType; }

//-----------------------------------------------------------------------------

void Limit::SetOffset(double offset) { m_offset = offset; }

double Limit::GetOffset() const { return m_offset; }

//-----------------------------------------------------------------------------

void Limit::SetP0(const CartesianPoint& p0) { m_p0 = p0; }

shipxml::CartesianPoint Limit::GetP0() const { return m_p0; }

//-----------------------------------------------------------------------------

void Limit::SetP1(const CartesianPoint& p1) { m_p1 = p1; }

shipxml::CartesianPoint Limit::GetP1() const { return m_p1; }

}  // namespace shipxml
