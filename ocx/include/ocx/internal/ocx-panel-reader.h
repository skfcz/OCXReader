//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCX_INCLUDE_OCX_OCX_PANEL_READER_H_
#define OCX_INCLUDE_OCX_OCX_PANEL_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <memory>

#include "ocx/internal/ocx-curve-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-surface-reader.h"

class OCXPanelReader {
 public:
  explicit OCXPanelReader(std::shared_ptr<OCXContext> ctx);

  TopoDS_Shape ReadPanels(LDOM_Element &vesselN);

 private:
  std::shared_ptr<OCXContext> ctx;

  TopoDS_Shape ReadPanel(LDOM_Element &panelN);

  TopoDS_Wire ReadPanelOuterContour(LDOM_Element &panelN);
  TopoDS_Shape ReadPanelSurface(LDOM_Element &panelN,
                                TopoDS_Wire &outerContour);

  TopoDS_Shape ReadPlates(LDOM_Element &panelN, TopoDS_Shape &referenceSurface);
  TopoDS_Shape ReadPlate(LDOM_Element &plateN, TopoDS_Shape &referenceSurface);

  TopoDS_Shape ReadStiffeners(LDOM_Element &panelN);
  TopoDS_Shape ReadStiffener(LDOM_Element &stiffenerN);

  TopoDS_Shape ReadBrackets(LDOM_Element &panelN);
  TopoDS_Shape ReadBracket(LDOM_Element &bracketN);
};

#endif  // OCX_INCLUDE_OCX_OCX_PANEL_READER_H_
