/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
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

#include "shipxml/internal/shipxml-enums.h"

#include <gp_Dir.hxx>

#include "shipxml/internal/shipxml-log.h"

namespace shipxml {

Standard_Real const AMSystemIsParallelTolerance = 45.0 / 180.0 * M_PI;

AMSystem ToAMSystem(gp_Dir const& normal) {
  //SHIPXML_DEBUG("ToAMSystem [{} {} {}]", normal.X(), normal.Y(), normal.Z())

  if (gp_Dir(1, 0, 0).IsParallel(normal, AMSystemIsParallelTolerance)) {
    return AMSystem::YZ;
  } else if (gp_Dir(0, 1, 0).IsParallel(normal, AMSystemIsParallelTolerance)) {
    return AMSystem::XZ;
  } else {
    return AMSystem::XZ;
  }
}

}  // namespace shipxml
