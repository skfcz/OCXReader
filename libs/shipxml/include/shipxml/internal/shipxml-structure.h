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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_STRUCTURE_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_STRUCTURE_H_

#include <list>

#include "shipxml/internal/shipxml-panel.h"

namespace shipxml {

class Structure {
 public:
  Structure() = default;
  ~Structure() = default;

  void AddPanel(shipxml::Panel const &panel);

  [[nodiscard]] std::list<shipxml::Panel> GetPanels() const;

 private:
  std::list<shipxml::Panel> m_panels;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_STRUCTURE_H_
