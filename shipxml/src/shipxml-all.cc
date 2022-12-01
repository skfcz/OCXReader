// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

// This file #includes all ocx implementation .cc files. The
// purpose is to allow a user to build ocx by compiling this
// file alone.

// The following lines pull in the real ocx*.cc files.
#include "src/AMCurve.cpp"
#include "src/ArcSegment.cpp"
#include "src/Bracket.cpp"
#include "src/CartesianPoint.cpp"
#include "src/EntityWithProperties.cpp"
#include "src/Extrusion.cpp"
#include "src/Limit.cpp"
#include "src/NamedEntity.cpp"
#include "src/Panel.cpp"
#include "src/Properties.cpp"
#include "src/ShipSteelTransfer.cpp"
#include "src/Structure.cpp"
#include "src/Support.cpp"
#include "src/ShipXMLDriver.cpp"

