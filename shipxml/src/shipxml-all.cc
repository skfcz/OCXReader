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
#include "AMCurve.cpp"
#include "ArcSegment.cpp"
#include "CartesianPoint.cpp"
#include "EntityWithProperties.cpp"
#include "Extrusion.cpp"
#include "Limit.cpp"
#include "NamedEntity.cpp"
#include "Panel.cpp"
#include "Properties.cpp"
#include "ShipSteelTransfer.cpp"
#include "Structure.cpp"
#include "Support.cpp"
#include "ShipXMLDriver.cpp"
#include "ShipXMLHelper.cc"
#include "GeneralData.cpp"
#include "PanelReader.cpp"
#include "KeyValue.cpp"
#include "shipxml-log.cc"


