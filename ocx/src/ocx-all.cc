//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

// This file #includes all ocx implementation .cc files. The
// purpose is to allow a user to build ocx by compiling this
// file alone.

// The following lines pull in the real ocx*.cc files.
#include "src/ocx-caf-control-reader.cc"
#include "src/ocx-context.cc"
#include "src/ocx-coordinate-system-reader.cc"
#include "src/ocx-curve-reader.cc"
#include "src/ocx-helper.cc"
#include "src/ocx-panel-reader.cc"
#include "src/ocx-panel.cc"
#include "src/ocx-reference-surfaces-reader.cc"
#include "src/ocx-surface-reader.cc"
#include "src/ocx-util.cc"
