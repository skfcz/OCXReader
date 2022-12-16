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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENUMS_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENUMS_H_

namespace shipxml {

enum AMSystem { XY, XZ, YZ };
enum LimitType {
  UNKNOWN,
  CURVE,
  TRACE,
  FOR_FLANGE,
  BLOCK,
  PANEL,
  PANEL_SAME_PLANE,
  STIFFENER,
  KNUCKLE,
  GAP,
  HOLE,
  PLANE
};
enum PanelType { PLANAR, KNUCKLED, SHELL_PANEL };
enum Orientation { UNDEFINED_O, FORE, AFT, PS, SB, TOP, BOTTOM };
enum LocationType { UNKNOWN_L, X_L, Y_L, Z_L, TP_L };
enum MajorPlane { UNDEFINED_M, X_M, Y_M, Z_M };

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENUMS_H_
