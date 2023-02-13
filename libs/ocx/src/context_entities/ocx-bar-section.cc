/***************************************************************************
 *   Created on: 29 Nov 2022                                               *
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

#include "ocx/internal/ocx-bar-section.h"

#include <utility>

namespace ocx::context_entities {

BarSection::BarSection(std::string barSectionType)
    : m_barSectionType(std::move(barSectionType)) {}

std::string BarSection::GetBarSectionType() const { return m_barSectionType; }

//-----------------------------------------------------------------------------

FlatBar::FlatBar(std::string const &barSectionType, Standard_Real width,
                 Standard_Real height)
    : BarSection(barSectionType), m_width(width), m_height(height) {}

Standard_Real FlatBar::GetWidth() const { return m_width; }

Standard_Real FlatBar::GetHeight() const { return m_height; }

//-----------------------------------------------------------------------------

RoundBar::RoundBar(std::string const &barSectionType, Standard_Real height,
                   Standard_Real diameter)
    : BarSection(barSectionType), m_height(height), m_diameter(diameter) {}

Standard_Real RoundBar::GetHeight() const { return m_height; }

Standard_Real RoundBar::GetDiameter() const { return m_diameter; }

}  // namespace ocx::context_entities
