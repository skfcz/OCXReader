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

#include <vector>
#include <string>

#include "shipxml/internal/shipxml-key-value.h"

namespace shipxml {

class Properties {
 public:
  Properties() = default;
  ~Properties() = default;

  shipxml::KeyValue Add(std::string const &key, std::string const &value);
  shipxml::KeyValue Add(std::string const &key, double value);
  shipxml::KeyValue Add(std::string const &key, int value);

  [[nodiscard]] std::vector<shipxml::KeyValue> GetValues() const;

  double GetWeight() const;
  void SetWeight(double weight);
  double GetArea() const;
  void SetArea(double area);

  const std::string &GetCog() const;
  void SetCog(const CartesianPoint cp);
  void SetCog(const std::string &mCog);
  const std::string &GetBbox0() const;
  void SetBbox0(const CartesianPoint cp);
  void SetBbox0(const std::string &mBbox0);
  const std::string &GetBbox1() const;

  void SetBbox1(const CartesianPoint cp);
  void SetBbox1(const std::string &mBbox1);

 private:
  std::vector<shipxml::KeyValue> m_values;
  std::string m_cog;
  std::string m_bbox0;
  std::string m_bbox1;
  double weight;
  double area;


};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PROPERTIES_H_
