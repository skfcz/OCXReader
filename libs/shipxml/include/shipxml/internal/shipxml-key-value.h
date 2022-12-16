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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_KEY_VALUE_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_KEY_VALUE_H_

#include <string>

namespace shipxml {

class KeyValue {
 public:
  KeyValue(std::string_view key, std::string_view value);

  [[nodiscard]] std::string GetKey() const;

  [[nodiscard]] std::string GetValue() const;

 private:
  std::string m_key;
  std::string m_value;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_KEY_VALUE_H_
