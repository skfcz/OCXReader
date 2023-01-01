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

#include "shipxml/internal/shipxml-properties.h"

#include <list>
#include <string>

#include "shipxml/internal/shipxml-key-value.h"

namespace shipxml {

shipxml::KeyValue Properties::Add(std::string const &key,
                                  std::string const &value) {
  shipxml::KeyValue kv(key, value);
  m_values.push_back(kv);

  return kv;
}

shipxml::KeyValue Properties::Add(std::string const &key, double value) {
  return Add(key, std::to_string(value));
}

shipxml::KeyValue Properties::Add(std::string const &key, int value) {
  return Add(key, std::to_string(value));
}

//-----------------------------------------------------------------------------

std::vector<shipxml::KeyValue> Properties::GetValues() const { return m_values; }
double Properties::GetWeight() const { return weight; }
void Properties::SetWeight(double weight) { Properties::weight = weight; }
double Properties::GetArea() const { return area; }
void Properties::SetArea(double area) { Properties::area = area; }
const std::string &Properties::GetCog() const { return m_cog; }
void Properties::SetCog(const std::string &mCog) { m_cog = mCog; }
const std::string &Properties::GetBbox0() const { return m_bbox0; }
void Properties::SetBbox0(const std::string &mBbox0) { m_bbox0 = mBbox0; }
const std::string &Properties::GetBbox1() const { return m_bbox1; }
void Properties::SetBbox1(const std::string &mBbox1) { m_bbox1 = mBbox1; }
void Properties::SetCog(const CartesianPoint cp) {
  SetCog(ToString(cp));
}
void Properties::SetBbox0(const CartesianPoint cp) {
  SetBbox0(ToString(cp));
}
void Properties::SetBbox1(const CartesianPoint cp) {
  SetBbox1(ToString(cp));
}

}  // namespace shipxml
