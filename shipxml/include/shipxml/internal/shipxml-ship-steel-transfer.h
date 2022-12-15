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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SHIP_STEEL_TRANSFER_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SHIP_STEEL_TRANSFER_H_

#include <memory>

#include "shipxml/internal/shipxml-structure.h"

namespace shipxml {

class ShipSteelTransfer {
 public:
  ShipSteelTransfer();
  ~ShipSteelTransfer() = default;

  [[nodiscard]] std::shared_ptr<Structure> GetStructure() const;

 private:
  std::shared_ptr<Structure> m_structure;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_SHIP_STEEL_TRANSFER_H_
