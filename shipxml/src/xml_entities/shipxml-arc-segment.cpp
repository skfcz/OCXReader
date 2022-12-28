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

#include "shipxml/internal/shipxml-arc-segment.h"

#include "shipxml/internal/shipxml-cartesian-point.h"

namespace shipxml {

ArcSegment::ArcSegment(shipxml::CartesianPoint pS, shipxml::CartesianPoint pE)
    : m_p0(pS), m_p1(pE), m_witherShins(false), m_isLine(true) {}

//-----------------------------------------------------------------------------

ArcSegment::ArcSegment(shipxml::CartesianPoint pS, shipxml::CartesianPoint pE,
                       shipxml::CartesianPoint pOnCircle,
                       shipxml::CartesianPoint pCenter, bool counterClockWise)
    : m_p0(pS),
      m_p1(pE),
      m_pM(pOnCircle),
      m_pC(pCenter),
      m_witherShins(counterClockWise),
      m_isLine(false) {}

bool ArcSegment::IsLine() { return m_isLine; }
bool ArcSegment::IsWithershins() { return m_witherShins; }

CartesianPoint ArcSegment::GetStartPoint() { return m_p0; }
CartesianPoint ArcSegment::GetEndPoint() { return m_p0; }
CartesianPoint ArcSegment::GetCenterPoint() { return m_pC; }
CartesianPoint ArcSegment::GetPointOnCircle() { return m_pM; }

}  // namespace shipxml
