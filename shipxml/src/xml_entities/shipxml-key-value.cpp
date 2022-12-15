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

#include "shipxml/internal/shipxml-key-value.h"

#include <string>

namespace shipxml {

KeyValue::KeyValue(std::string_view k, std::string_view v)
    : m_key(k), m_value(v) {}

//-----------------------------------------------------------------------------

std::string KeyValue::GetKey() const { return m_key; }

std::string KeyValue::GetValue() const { return m_value; }

}  // namespace shipxml
