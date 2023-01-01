/***************************************************************************
 *   Created on: 31 Dec 2022                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_READER_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_READER_H_

#include <LDOM_Element.hxx>
#include <memory>

#include "shipxml/internal/shipxml-panel.h"
#include "shipxml/internal/shipxml-plate.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"

namespace shipxml {

class PlateReader {
 public:
  PlateReader();

  ~PlateReader() = default;

  void ReadPlates(LDOM_Element const &ocx_panelN, Panel &panel) const;




 private:
  [[nodiscard]] static shipxml::Plate ReadPlate(LDOM_Element const &plateN, shipxml::Panel &panel);

  [[nodiscard]] static bool ReadOuterContour(LDOM_Element const &plateN);
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_READER_H_
