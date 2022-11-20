//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCX_INCLUDE_OCX_OCX_CURVE_READER_H_
#define OCX_INCLUDE_OCX_OCX_CURVE_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <memory>

#include "ocx/internal/ocx-context.h"

namespace ocx {

/**
 * This class is used to read OCX XML for curves and create OpenCascade
 * TopoDS_Wires.
 */
class OCXCurveReader {
 public:
  /**
   * Create a new curve reader
   *
   * @param ctx the context used to lookup scaling factor
   */
  explicit OCXCurveReader(std::shared_ptr<OCXContext> ctx)
      : ctx(std::move(ctx)){};

  /**
   * Base method managing the reading of various curve types, e.g.
   * Ellipse3D, CircumCircle3D, Circle3D, CircumArc3D, Line3D, CompositeCurve3D,
   * PolyLine3D, NURBS3D
   *
   * @param surfaceN the surface element
   * @return the created TopoDS_Wire or TopoDS_Edge
   */
  [[nodiscard]] TopoDS_Wire ReadCurve(LDOM_Element const &curveParentN) const;

 private:
  /**
   * The context of the reader
   */
  std::shared_ptr<OCXContext> ctx;

  [[nodiscard]] TopoDS_Shape ReadCompositeCurve3D(LDOM_Element const &curveN,
                                                  std::string_view id,
                                                  std::string_view guid) const;

  [[nodiscard]] TopoDS_Wire ReadEllipse3D(LDOM_Element const &curveN,
                                          std::string_view id,
                                          std::string_view guid) const;

  [[nodiscard]] TopoDS_Wire ReadCircumCircle3D(LDOM_Element const &circleN,
                                               std::string_view id,
                                               std::string_view guid) const;

  [[nodiscard]] TopoDS_Wire ReadCircle3D(LDOM_Element const &circleN,
                                         std::string_view id,
                                         std::string_view guid) const;

  [[nodiscard]] TopoDS_Edge ReadCircumArc3D(LDOM_Element const &curveN,
                                            std::string_view id,
                                            std::string_view guid) const;

  [[nodiscard]] TopoDS_Edge ReadLine3D(LDOM_Element const &lineN,
                                       std::string_view id,
                                       std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadPolyLine3D(LDOM_Element const &curveN,
                                            std::string_view id,
                                            std::string_view guid) const;

  [[nodiscard]] TopoDS_Shape ReadNURBS3D(LDOM_Element const &curveN,
                                         std::string_view id,
                                         std::string_view guid) const;
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_CURVE_READER_H_
