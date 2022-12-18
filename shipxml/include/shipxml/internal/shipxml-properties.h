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

 private:
  std::vector<shipxml::KeyValue> m_values;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PROPERTIES_H_
