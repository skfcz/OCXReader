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

#include "shipxml/internal/shipxml-support.h"

#include <string>

namespace shipxml {

void Support::SetGrid(std::string_view grid) { m_grid = grid; }

std::string Support::GetGrid() const { return m_grid; }

//-----------------------------------------------------------------------------

void Support::SetCoordinate(std::string_view coordinate) {
  m_coordinate = coordinate;
}

std::string Support::GetCoordinate() const { return m_coordinate; }

//-----------------------------------------------------------------------------

void Support::SetOrientation(Orientation orientation) {
  m_orientation = orientation;
}

Orientation Support::GetOrientation() const { return m_orientation; }

//-----------------------------------------------------------------------------

void Support::SetIsPlanar(bool isPlanar) { m_planar = isPlanar; }

bool Support::IsPlanar() const { return m_planar; }

//-----------------------------------------------------------------------------

void Support::SetLocationType(LocationType locationType) {
  m_locationType = locationType;
}

LocationType Support::GetLocationType() const { return m_locationType; }

//-----------------------------------------------------------------------------

void Support::SetMajorPlane(MajorPlane majorPlane) {
  m_majorPlane = majorPlane;
}

MajorPlane Support::GetMajorPlane() const { return m_majorPlane; }

//-----------------------------------------------------------------------------

void Support::SetTP1(CartesianPoint cartesianPoint) { m_tp1 = cartesianPoint; }

CartesianPoint Support::GetTP1() const { return m_tp1; }

//-----------------------------------------------------------------------------

void Support::SetTP2(CartesianPoint cartesianPoint) { m_tp2 = cartesianPoint; }

CartesianPoint Support::GetTP2() const { return m_tp2; }

//-----------------------------------------------------------------------------

void Support::SetTP3(CartesianPoint cartesianPoint) { m_tp3 = cartesianPoint; }

CartesianPoint Support::GetTP3() const { return m_tp3; }

//-----------------------------------------------------------------------------

void Support::SetNormal(Vector n) { m_normal = n; }

Vector Support::GetNormal() const { return m_normal; }

}  // namespace shipxml
