/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

// This file #includes all ocx implementation .cc files. The
// purpose is to allow a user to build ocx by compiling this
// file alone.
// The following lines pull in the real ocx*.cc files.

// General
#include "src/ocx-context.cc"
#include "src/ocx-helper.cc"
#include "src/ocx-log.cc"
#include "src/ocx-reader.cc"
#include "src/ocx-util.cc"

// Shared
#include "src/shared/ocx-curve.cc"
#include "src/shared/ocx-limited-by.cc"
#include "src/shared/ocx-outer-contour.cc"
#include "src/shared/ocx-surface.cc"
#include "src/shared/ocx-unbounded-geometry.cc"

// <ocx:ocxXML>
//    <ocx:ClassCatalogue>
#include "src/class_catalogue/ocx-class-catalogue.cc"
#include "src/class_catalogue/x_section_catalogue/ocx-bar-section.cc"
#include "src/class_catalogue/x_section_catalogue/ocx-x-section-catalogue.cc"
//    </ocx:ClassCatalogue>
//    <ocx:Vessel>
#include "src/vessel/ocx-vessel.cc"
//      <ocx:CoordinateSystem>
#include "src/vessel/coordinate_system/ocx-coordinate-system.cc"
//      </ocx:CoordinateSystem>
//      <ocx:Panel>
#include "src/vessel/panel/composed_of/ocx-composed-of.cc"
#include "src/vessel/panel/ocx-panel.cc"
#include "src/vessel/panel/stiffened_by/ocx-stiffened-by.cc"
//      </ocx:Panel>
//      <ocx:ReferenceSurfaces>
#include "src/vessel/reference_surfaces/ocx-reference-surfaces.cc"
//      </ocx:ReferenceSurfaces>
//    </ocx:Vessel>
// </ocx:ocxXML>
