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

#include "shipxml/internal/shipxml-entity-with-properties.h"

#include <string>

#include "shipxml/internal/shipxml-named-entity.h"

namespace shipxml {

EntityWithProperties::EntityWithProperties(std::string_view name)
    : NamedEntity(name) {}

//-----------------------------------------------------------------------------

shipxml::Properties EntityWithProperties::GetProperties() const {
  return m_properties;
}

}  // namespace shipxml
