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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_AM_CURVE_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_AM_CURVE_H_

#include <vector>

#include "shipxml/internal/shipxml-arc-segment.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

class AMCurve {
 public:
  explicit AMCurve(AMSystem system);

  [[nodiscard]] std::vector<ArcSegment> GetSegments() const;

  [[nodiscard]] AMSystem GetSystem() const;

 private:
  std::vector<ArcSegment> m_segments;
  AMSystem m_system;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_AM_CURVE_H_
