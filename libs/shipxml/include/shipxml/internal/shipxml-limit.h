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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LIMIT_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LIMIT_H_

#include <string>

#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-cartesian-point.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-named-entity.h"

namespace shipxml {

class Limit : public NamedEntity {
 public:
  explicit Limit(std::string_view name);

  void SetContour(AMCurve const& contour);
  [[nodiscard]] AMCurve GetContour() const;

  void SetFeature(std::string_view feature);
  [[nodiscard]] std::string GetFeature() const;

  void SetLimitType(LimitType limitType);
  [[nodiscard]] LimitType GetLimitType() const;

  void SetOffset(double offset);
  [[nodiscard]] double GetOffset() const;

  void SetP0(CartesianPoint const& p0);
  [[nodiscard]] CartesianPoint GetP0() const;

  void SetP1(CartesianPoint const& p1);
  [[nodiscard]] CartesianPoint GetP1() const;

 private:
  AMCurve m_contour = AMCurve(AMSystem::XY);
  std::string m_feature;
  LimitType m_limitType = LimitType::UNKNOWN;
  double m_offset{};
  CartesianPoint m_p0;
  CartesianPoint m_p1;
};
}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LIMIT_H_
