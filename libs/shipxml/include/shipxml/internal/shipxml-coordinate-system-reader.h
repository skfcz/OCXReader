/***************************************************************************
 *   Created on: 11 Dec 2022                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_COORDINATE_SYSTEM_READER_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_COORDINATE_SYSTEM_READER_H_

#include <LDOM_Element.hxx>
#include <memory>

#include "shipxml/internal/shipxml-panel.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"

namespace shipxml {

class CoordinateSystemReader {
 public:
  CoordinateSystemReader(LDOM_Element const &vesselN,
                         std::shared_ptr<ShipSteelTransfer> sst);

  ~CoordinateSystemReader() = default;

  void ReadCoordinateSystem() const;

 private:
  std::shared_ptr<ShipSteelTransfer> m_sst;
  LDOM_Element m_ocxVesselEL;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_COORDINATE_SYSTEM_READER_H_