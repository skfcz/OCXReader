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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXTRUSION_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXTRUSION_H_

#include "shipxml/internal/shipxml-entity-with-properties.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

class Extrusion : public EntityWithProperties {
 public:
  Extrusion();
  ~Extrusion() = default;

  void SetThickness(double thickness);

  [[nodiscard]] double GetThickness() const;

  void SetOffset(double offset);

  [[nodiscard]] double GetOffset() const;

  void SetOrientation(shipxml::Orientation orientation);

  [[nodiscard]] shipxml::Orientation GetOrientation() const;

 private:
  double m_thickness{};
  double m_offset{};
  shipxml::Orientation m_orientation = UNDEFINED_O;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXTRUSION_H_
