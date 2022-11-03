//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADERLIB_INCLUDE_OCX_OCX_REFERENCE_SURFACES_READER_H_
#define OCXREADERLIB_INCLUDE_OCX_OCX_REFERENCE_SURFACES_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>

#include "ocx/internal/ocx-context.h"

class OCXReferenceSurfacesReader {
 public:
  OCXReferenceSurfacesReader(OCXContext *ctx);

  /**
   * Read the references surfaces from the OCX file.
   * The references surfaces are registered in the OCXContext as TopoDS_Shell.
   * @param vesselN the Vessel element
   * @return an assembly containing all reference surface
   */
  TopoDS_Shape ReadReferenceSurfaces(LDOM_Element &vesselN);

 private:
  OCXContext *ctx;
};

#endif  // OCXREADERLIB_INCLUDE_OCX_OCX_REFERENCE_SURFACES_READER_H_
