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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CARTESIAN_POINT_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CARTESIAN_POINT_H_

namespace shipxml {

class CartesianPoint {
 public:
  CartesianPoint();
  CartesianPoint(double x, double y, double z);
  
  ~CartesianPoint() = default;

  void SetX(double value);
  [[nodiscard]] double GetX() const;

  void SetY(double value);
  [[nodiscard]] double GetY() const;

  void SetZ(double value);
  [[nodiscard]] double GetZ() const;

 private:
  double m_x{};
  double m_y{};
  double m_z{};
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CARTESIAN_POINT_H_
