//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADERLIB_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_
#define OCXREADERLIB_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>

#include "ocx/internal/ocx-context.h"

class OCXCoordinateSystemReader {
 public:
  OCXCoordinateSystemReader(OCXContext *ctx);
  TopoDS_Shape ReadCoordinateSystem(LDOM_Element &vesselN);

 private:
  OCXContext *ctx;

  TopoDS_Shape ReadRefPlanes(LDOM_Element &refPlanesN);
};

#endif  // OCXREADERLIB_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_
