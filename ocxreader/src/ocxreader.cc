/***************************************************************************
 *   Created on: 15 Nov 2022                                               *
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

// This file #includes all ocxreader implementation .cc files. The
// purpose is to allow a user to build ocx by compiling this
// file alone.

// The following lines pull in the real ocxreader*.cc files.

#include "src/main.cc"
#include "src/ocxreader-cli.cc"
#include "src/ocxreader-export.cc"
#include "src/ocxreader-log.cc"
