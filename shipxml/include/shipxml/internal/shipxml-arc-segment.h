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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ARC_SEGMENT_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ARC_SEGMENT_H_

#include "shipxml/internal/shipxml-cartesian-point.h"

namespace shipxml {

class ArcSegment {
 public:
  ArcSegment(shipxml::CartesianPoint pS, shipxml::CartesianPoint pE);
  ArcSegment(shipxml::CartesianPoint p0, shipxml::CartesianPoint p1,
             shipxml::CartesianPoint pM, shipxml::CartesianPoint pC,
             bool counterClockWise);

 private:
  CartesianPoint m_p0;
  CartesianPoint m_p1;
  CartesianPoint m_pM;
  CartesianPoint m_pC;
  bool m_witherShins;
  bool m_isLine;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ARC_SEGMENT_H_
