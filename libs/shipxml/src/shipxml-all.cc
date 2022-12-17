/***************************************************************************
 *   Created on: 30 Nov 2022                                               *
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

// This file #includes all shipxml implementation .cc files. The
// purpose is to allow a user to build ocx by compiling this
// file alone.

// The following lines pull in the real shipxml*.cc files.

#include "src/reader/shipxml-coordinate-system-reader.cc"
#include "src/reader/shipxml-panel-reader.cc"
#include "src/shipxml-driver.cc"
#include "src/shipxml-helper.cc"
#include "src/shipxml-log.cc"
#include "src/shipxml-ship-steel-transfer.cc"
#include "src/xml_entities/shipxml-am-curve.cc"
#include "src/xml_entities/shipxml-arc-segment.cc"
#include "src/xml_entities/shipxml-cartesian-point.cc"
#include "src/xml_entities/shipxml-entity-with-properties.cc"
#include "src/xml_entities/shipxml-extrusion.cc"
#include "src/xml_entities/shipxml-generel-data.cc"
#include "src/xml_entities/shipxml-key-value.cc"
#include "src/xml_entities/shipxml-limit.cc"
#include "src/xml_entities/shipxml-named-entity.cc"
#include "src/xml_entities/shipxml-panel.cc"
#include "src/xml_entities/shipxml-properties.cc"
#include "src/xml_entities/shipxml-structure.cc"
#include "src/xml_entities/shipxml-support.cc"
