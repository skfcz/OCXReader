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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_NAMED_ENTITY_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_NAMED_ENTITY_H_

#include <string>

namespace shipxml {

class NamedEntity {
 public:
  explicit NamedEntity(std::string_view name);

  ~NamedEntity() = default;

  void SetName(std::string_view name);

  [[nodiscard]] std::string GetName() const;

 private:
  std::string m_name;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_NAMED_ENTITY_H_
