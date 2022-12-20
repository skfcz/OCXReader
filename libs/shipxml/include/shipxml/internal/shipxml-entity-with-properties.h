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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENTITY_WITH_PROPERTIES_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENTITY_WITH_PROPERTIES_H_

#include <string>

#include "shipxml/internal/shipxml-named-entity.h"
#include "shipxml/internal/shipxml-properties.h"

namespace shipxml {

class EntityWithProperties : public NamedEntity {
 public:
  explicit EntityWithProperties(std::string_view name);

  [[nodiscard]] shipxml::Properties &GetProperties();

 private:
  shipxml::Properties m_properties;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_ENTITY_WITH_PROPERTIES_H_
