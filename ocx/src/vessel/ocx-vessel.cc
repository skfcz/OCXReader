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

#include "ocx/internal/ocx-vessel.h"

#include <LDOM_Element.hxx>

#include "ocx/internal/ocx-context.h"
#include "ocx/internal/ocx-coordinate-system.h"
#include "ocx/internal/ocx-panel.h"
#include "ocx/internal/ocx-reference-surfaces.h"

namespace ocx::vessel {

void ReadVessel() {
  LDOM_Element vesselN = ocx::helper::GetFirstChild(
      OCXContext::GetInstance()->OCXRoot(), "Vessel");
  if (vesselN.isNull()) {
    OCX_ERROR("No Vessel child node found.");
    return;
  }

  // Read coordinate system
  ocx::vessel::coordinate_system::ReadCoordinateSystem(vesselN);

  // Read reference surfaces
  ocx::vessel::reference_surfaces::ReadReferenceSurfaces(vesselN);

  // Read panels
  ocx::vessel::panel::ReadPanels(vesselN);
}

}  // namespace ocx::vessel
