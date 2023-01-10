/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_VECTOR_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_VECTOR_H_

#include <gp_Dir.hxx>
#include <string>

namespace shipxml {

class Vector {
 public:
  Vector();
  Vector(double x, double y, double z);
  Vector(gp_Dir const& dir);

  ~Vector() = default;

  void SetX(double value);
  [[nodiscard]] double GetX() const;

  void SetY(double value);
  [[nodiscard]] double GetY() const;

  void SetZ(double value);
  [[nodiscard]] double GetZ() const;

  [[nodiscard]] gp_Dir ToDir() const;

  [[nodiscard]] std::string ToString() const;

 private:
  double m_x{};
  double m_y{};
  double m_z{};
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_VECTOR_H_
