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

  std::cout << "add KeyValue #" << m_values.size() << " to Properties " << std::endl;


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

}  // namespace shipxml
