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

#include <math.h>

namespace shipxml {

shipxml::AMSystem ToAMSystem(gp_Dir normal) {
  OCX_INFO("ToAMSystem [{} {} {}]", normal.X(), normal.Y(), normal.Z());

  if (gp_Dir(1, 0, 0).IsParallel(normal, 45.0 / 180.0 * M_PI)) {
    return YZ;
  } else if (gp_Dir(0, 1, 0).IsParallel(normal, 45.0 / 180.0 * M_PI)) {
    return XZ;
  } else {
    return XZ;
  }
}

shipxml::CartesianPoint Convert(gp_Pnt point) {
  return shipxml::CartesianPoint(point.X(), point.Y(), point.Z());
}

shipxml::Vector Convert(gp_Dir vec) {
  return shipxml::Vector(vec.X(), vec.Y(), vec.Z());
}
gp_Dir Convert(shipxml::Vector v) {
  return gp_Dir( v.GetX(), v.GetY(), v.GetZ());
}

std::string ToString(double d) {
  std::ostringstream strs;
  strs <<  d;
  return strs.str();
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

std::string ToString(shipxml::AMSystem p) {
  switch (p) {
    case XZ:
      return "XZ";
    case YZ:
      return "YZ";
    case XY:
      return "XY";
  }

  return "UNDEFINED";
}


}  // namespace shipxml
