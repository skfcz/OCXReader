// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_PANEL_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_PANEL_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <memory>

#include "ocx/internal/ocx-curve-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-surface-reader.h"

namespace ocx {

class OCXPanelReader {
 public:
  explicit OCXPanelReader(std::shared_ptr<OCXContext> ctx)
      : ctx(std::move(ctx)) {}

  TopoDS_Shape ReadPanels(LDOM_Element const &vesselN) const;

 private:
  /**
   * The context of the reader
   */
  std::shared_ptr<OCXContext> ctx;

  [[nodiscard]] TopoDS_Shape ReadPanel(LDOM_Element const &panelN,
                                       std::string_view id,
                                       std::string_view guid) const;

  [[nodiscard]] TopoDS_Wire ReadPanelOuterContour(LDOM_Element const &panelN,
                                                  std::string_view id,
                                                  std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadPanelSurface(LDOM_Element const &panelN,
                                              TopoDS_Wire const &outerContour,
                                              std::string_view id,
                                              std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadPlates(LDOM_Element const &panelN,
                                        TopoDS_Shape const &referenceSurface,
                                        std::string_view id,
                                        std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadPlate(LDOM_Element const &plateN,
                                       TopoDS_Shape const &referenceSurface,
                                       std::string_view id,
                                       std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadStiffeners(LDOM_Element const &panelN,
                                            std::string_view id,
                                            std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadStiffener(LDOM_Element const &stiffenerN,
                                           std::string_view id,
                                           std::string_view guid) const;

  TopoDS_Shape ReadBrackets(LDOM_Element &panelN);
  TopoDS_Shape ReadBracket(LDOM_Element &bracketN);
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_PANEL_READER_H_
