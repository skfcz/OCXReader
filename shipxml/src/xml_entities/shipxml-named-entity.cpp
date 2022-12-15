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

#include "shipxml/internal/shipxml-named-entity.h"

#include <string>

namespace shipxml {

NamedEntity::NamedEntity(std::string_view name) : m_name(name) {}

//-----------------------------------------------------------------------------

void NamedEntity::SetName(std::string_view name) { m_name = name; }

std::string NamedEntity::GetName() const { return m_name; }

}  // namespace shipxml
