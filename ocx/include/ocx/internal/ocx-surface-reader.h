//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADERLIB_INCLUDE_OCX_OCX_SURFACE_READER_H_
#define OCXREADERLIB_INCLUDE_OCX_OCX_SURFACE_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <string>
#include <memory>

#include "ocx/internal/ocx-context.h"

class OCXSurfaceReader {
 public:
  explicit OCXSurfaceReader(std::shared_ptr<OCXContext> ctx);
  TopoDS_Shape ReadSurface(LDOM_Element &vesselN);

 private:
  std::shared_ptr<OCXContext> ctx;

  TopoDS_Face ReadCone3D(LDOM_Element &surfaceN,
                         std::string guid,
                         std::string id);
  TopoDS_Face ReadCylinder3D(LDOM_Element &surfaceN,
                             std::string guid,
                             std::string id);
  TopoDS_Face ReadExtrudedSurface(LDOM_Element &surfaceN,
                                  std::string guid,
                                  std::string id);
  TopoDS_Face ReadNURBSurface(LDOM_Element &nurbsSrfN,
                              std::string guid,
                              std::string id);
  TopoDS_Face ReadPlane3D(LDOM_Element &surfaceN,
                          std::string guid,
                          std::string id);
  TopoDS_Face ReadSphere3D(LDOM_Element &surfaceN,
                           std::string guid,
                           std::string id);
  TopoDS_Shell ReadSurfaceCollection(LDOM_Element &surfaceN,
                                     std::string guid,
                                     std::string id);
};

#endif  // OCXREADERLIB_INCLUDE_OCX_OCX_SURFACE_READER_H_
