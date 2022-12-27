/***************************************************************************
 *   Created on: 13 Dec 2022                                               *
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

#include "shipxml/internal/shipxml-helper.h"

namespace  shipxml {


  std::vector<shipxml::ArcSegment> ReadCurve(const LDOM_Element &curveRootN) {
    auto segments = std::vector<shipxml::ArcSegment>();



    return segments;
  }

  shipxml::CartesianPoint Convert(gp_Pnt point) {
    return shipxml::CartesianPoint(point.X(), point.Y(), point.Z());
  }


  shipxml::Vector Convert(gp_Dir vec) {
    return shipxml::Vector(vec.X(), vec.Y(), vec.Z());
  }

  std::string ToString(shipxml::CartesianPoint p) {

    std::ostringstream strs;
    strs << "(" << p.GetX() << ", " << p.GetY() << ", " << p.GetZ() << ")";
    return strs.str();

  }

  std::string ToString(shipxml::Vector v) {

    std::ostringstream strs;
    strs << "(" << v.GetX() << ", " << v.GetY() << ", " << v.GetZ() << ")";
    return strs.str();

  }

  std::string ToString(shipxml::Orientation o) {
    switch (o) {
      case FORE:
        return "FORE";
      case AFT:
        return "AFT";
      case PS:
        return "PS";
      case SB:
        return "SB";
      case TOP:
        return "TOP";
      case BOTTOM:
        return "BOTTOM";
    }

    return "UNDEFINED";
  }

  std::string ToString(shipxml::LocationType l) {
    switch (l) {
      case X_L:
        return "X";
      case Y_L:
        return "Y";
      case Z_L:
        return "Z";
    }

    return "TP";
  }

  std::string ToString(shipxml::MajorPlane p) {
    switch (p) {
      case X_M:
        return "X";
      case Y_M:
        return "Y";
      case Z_M:
        return "Z";
    }

    return "UNDEFINED";
  }



}


