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

#include "shipxml/internal/shipxml-am-curve.h"

#include <vector>

#include "shipxml/internal/shipxml-arc-segment.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

AMCurve::AMCurve(AMSystem system) : m_system(system) {}

//-----------------------------------------------------------------------------

void AMCurve::AddSegment(ArcSegment const &segment) {
  m_segments.push_back(segment);
}

void AMCurve::AddSegments(std::vector<ArcSegment> const &segments) {
  m_segments.insert(m_segments.end(), segments.begin(), segments.end());
}

std::vector<ArcSegment> AMCurve::GetSegments() const { return m_segments; }

//-----------------------------------------------------------------------------

void AMCurve::SetSystem(AMSystem const &system) { m_system = system; }

AMSystem AMCurve::GetSystem() const { return m_system; }

}  // namespace shipxml
