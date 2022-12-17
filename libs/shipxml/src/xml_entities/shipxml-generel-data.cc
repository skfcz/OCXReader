/***************************************************************************
 *   Created on: 02 Dec 2022                                               *
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

#include "shipxml/internal/shipxml-generel-data.h"

namespace shipxml {

void GeneralData::SetShipName(std::string_view shipName) {
  m_shipName = shipName;
}

std::string GeneralData::GetShipName() const { return m_shipName; }

//-----------------------------------------------------------------------------

void GeneralData::SetYardNumber(std::string_view yardNumber) {
  m_yardNumber = yardNumber;
}

std::string GeneralData::GetYardNumber() const { return m_yardNumber; }

//-----------------------------------------------------------------------------

void GeneralData::SetAftEnd(double value) { m_aftEnd = value; }

double GeneralData::GetAftEnd() const { return m_aftEnd; }

//-----------------------------------------------------------------------------

void GeneralData::SetAftPerpendicular(double value) {
  m_aftPerpendicular = value;
}

double GeneralData::GetAftPerpendicular() const { return m_aftPerpendicular; }

//-----------------------------------------------------------------------------

void GeneralData::SetForwardEnd(double value) { m_forwardEnd = value; }

double GeneralData::GetForwardEnd() const { return m_forwardEnd; }

//-----------------------------------------------------------------------------

void GeneralData::SetForwardPerpendicular(double value) {
  m_forwardPerpendicular = value;
}

double GeneralData::GetForwardPerpendicular() const {
  return m_forwardPerpendicular;
}

//-----------------------------------------------------------------------------

void GeneralData::SetLengthOverall(double value) { m_lengthOverall = value; }

double GeneralData::GetLengthOverall() const { return m_lengthOverall; }

//-----------------------------------------------------------------------------

void GeneralData::SetMaxDraught(double value) { m_maxDraught = value; }

double GeneralData::GetMaxDraught() const { return m_maxDraught; }

//-----------------------------------------------------------------------------

void GeneralData::SetMaxBreadth(double value) { m_maxBreadth = value; }

double GeneralData::GetMaxBreadth() const { return m_maxBreadth; }

//-----------------------------------------------------------------------------

void GeneralData::SetMaxHeight(double value) { m_maxHeight = value; }

double GeneralData::GetMaxHeight() const { return m_maxHeight; }

//-----------------------------------------------------------------------------

void GeneralData::SetHeightOfMainDeck(double value) {
  m_heightOfMainDeck = value;
}

double GeneralData::GetHeightOfMainDeck() const { return m_heightOfMainDeck; }

}  // namespace shipxml
