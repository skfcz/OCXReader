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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SUPPORT_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SUPPORT_H_

#include <string>

#include "shipxml-vector.h"
#include "shipxml/internal/shipxml-cartesian-point.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

class Support {
 public:
  Support() = default;
  ~Support() = default;

  void SetGrid(std::string_view grid);
  [[nodiscard]] std::string GetGrid() const;

  void SetCoordinate(std::string_view coordinate);
  [[nodiscard]] std::string GetCoordinate() const;

  void SetOrientation(Orientation orientation);
  [[nodiscard]] Orientation GetOrientation() const;

  void SetIsPlanar(bool isPlanar);
  [[nodiscard]] bool IsPlanar() const;

  void SetLocationType(LocationType locationType);
  [[nodiscard]] LocationType GetLocationType() const;

  void SetMajorPlane(MajorPlane majorPlane);
  [[nodiscard]] MajorPlane GetMajorPlane() const;

  void SetTP1(CartesianPoint cartesianPoint);
  [[nodiscard]] CartesianPoint GetTP1() const;

  void SetTP2(CartesianPoint cartesianPoint);
  [[nodiscard]] CartesianPoint GetTP2() const;

  void SetTP3(CartesianPoint cartesianPoint);
  [[nodiscard]] CartesianPoint GetTP3() const;
  
  void SetNormal(Vector n);
  [[nodiscard]] Vector GetNormal() const;

 private:
  std::string m_grid;
  std::string m_coordinate;
  Orientation m_orientation = Orientation::UNDEFINED;
  bool m_planar = false;
  LocationType m_locationType = LocationType::UNKNOWN;
  MajorPlane m_majorPlane = MajorPlane::UNDEFINED;
  CartesianPoint m_tp1;
  CartesianPoint m_tp2;
  CartesianPoint m_tp3;
  Vector m_normal;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SUPPORT_H_
