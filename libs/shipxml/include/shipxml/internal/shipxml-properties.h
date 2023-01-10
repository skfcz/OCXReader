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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PROPERTIES_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PROPERTIES_H_

#include <string>
#include <vector>

#include "shipxml/internal/shipxml-cartesian-point.h"
#include "shipxml/internal/shipxml-key-value.h"

namespace shipxml {

class Properties {
 public:
  Properties() = default;
  ~Properties() = default;

  KeyValue Add(std::string const &key, std::string const &value);
  KeyValue Add(std::string const &key, double value);
  KeyValue Add(std::string const &key, int value);

  [[nodiscard]] std::vector<KeyValue> GetValues() const;

  void SetWeight(double const &weight);
  [[nodiscard]] double GetWeight() const;

  void SetArea(double const &area);
  [[nodiscard]] double GetArea() const;

  void SetCog(std::string_view mCog);
  void SetCog(CartesianPoint const &cp);
  [[nodiscard]] std::string GetCog() const;

  void SetBbox0(std::string_view mBbox0);
  void SetBbox0(CartesianPoint const &cp);
  [[nodiscard]] std::string GetBbox0() const;

  void SetBbox1(std::string_view mBbox1);
  void SetBbox1(CartesianPoint const &cp);
  [[nodiscard]] std::string GetBbox1() const;

 private:
  std::vector<KeyValue> m_values;
  std::string m_cog;
  std::string m_bbox0;
  std::string m_bbox1;
  double m_weight;
  double m_area;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PROPERTIES_H_
