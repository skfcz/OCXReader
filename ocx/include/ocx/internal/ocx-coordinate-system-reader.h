// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_
#define OCX_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_

#include <LDOM_Element.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <memory>
#include <utility>

#include "ocx/internal/ocx-context.h"

namespace ocx {

class OCXCoordinateSystemReader {
 public:
  explicit OCXCoordinateSystemReader(std::shared_ptr<OCXContext> ctx)
      : ctx(std::move(ctx)) {}

  TopoDS_Shape ReadCoordinateSystem(LDOM_Element const &vesselN);

 private:
  /**
   * The context of the reader
   */
  std::shared_ptr<OCXContext> ctx;

  TopoDS_Shape ReadRefPlane(LDOM_Element const &refPlanesN,
                            Quantity_Color const &color);
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_COORDINATE_SYSTEM_READER_H_
