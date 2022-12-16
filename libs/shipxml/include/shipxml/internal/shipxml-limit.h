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

  void SetContour(shipxml::AMCurve const& contour);

  [[nodiscard]] shipxml::AMCurve GetContour() const;

  void SetFeature(std::string_view feature);

  [[nodiscard]] std::string GetFeature() const;

  void SetLimitType(shipxml::LimitType limitType);

  [[nodiscard]] shipxml::LimitType GetLimitType() const;

  void SetOffset(double offset);

  [[nodiscard]] double GetOffset() const;

  void SetP0(shipxml::CartesianPoint const& p0);

  [[nodiscard]] shipxml::CartesianPoint GetP0() const;

  void SetP1(shipxml::CartesianPoint const& p1);

  [[nodiscard]] shipxml::CartesianPoint GetP1() const;

 private:
  shipxml::AMCurve m_contour = shipxml::AMCurve(XY);
  std::string m_feature;
  shipxml::LimitType m_limitType = UNKNOWN;
  double m_offset{};
  shipxml::CartesianPoint m_p0;
  shipxml::CartesianPoint m_p1;
};
}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LIMIT_H_
