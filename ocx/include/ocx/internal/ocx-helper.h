//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCX_INCLUDE_OCX_OCX_HELPER_H_
#define OCX_INCLUDE_OCX_OCX_HELPER_H_

#include <LDOM_Element.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <memory>
#include <vector>

#include "ocx/internal/ocx-context.h"

struct KnotMults {
  TColStd_Array1OfReal knots;
  TColStd_Array1OfInteger mults;
  bool IsNull;
};

struct PolesWeightsCurve {
  TColgp_Array1OfPnt poles;
  TColStd_Array1OfReal weights;
  bool IsNull = false;

  explicit PolesWeightsCurve(int const &num) : poles(1, num), weights(1, num) {}
};

// https://dev.opencascade.org/doc/refman/html/class_geom___b_spline_surface.html#a4ccceccdf053e7164b6a6ac38251b709
struct PolesWeightsSurface {
  TColgp_Array2OfPnt poles;
  TColStd_Array2OfReal weights;
  bool IsNull = false;

  PolesWeightsSurface(int const &uNum, int const &vNum)
      : poles(1, uNum, 1, vNum), weights(1, uNum, 1, vNum) {}
};

class OCXHelper {
 public:
  static std::string GetLocalTagName(const LDOM_Element &elem,
                                     bool keepPrefix = false);

  static std::string GetLocalAttrName(const LDOM_Node &node);

  static std::string GetChildTagName(const LDOM_Element &parent,
                                     bool keepPrefix = false);

  static LDOM_Element GetFirstChild(const LDOM_Element &parent,
                                    const std::string_view &localName);

  /**
   * Split a string by given delimiter and return a Container of type T
   * @see https://stackoverflow.com/a/1493195/13279727
   *
   * @tparam ContainerT Container type
   * @param str String to split
   * @param tokens Container to store the result
   * @param delimiters Delimiter used to split the string (default: " ")
   * @param trimEmpty Trim empty tokens (default: true)
   */
  template <class ContainerT>
  static void Tokenize(std::string_view str, ContainerT &tokens,
                       const std::string &delimiters = " ",
                       bool trimEmpty = true);

  /**
   * Get the double attribute value from an attribute. Does not care if this is
   * more an integer '50' or double '50.0'. If the attribute is missing the
   * value is not touched
   *
   * @param elem the element containing the attribute
   * @param attrName the attributes name
   * @param value the variable to put the value
   */
  static void GetDoubleAttribute(const LDOM_Element &elem,
                                 const std::string &attrName,
                                 Standard_Real &value);

  static gp_Pnt ReadPoint(const LDOM_Element &pointN,
                          const std::shared_ptr<OCXContext> &ctx);

  static gp_Dir ReadDirection(const LDOM_Element &dirN);

  static double ReadDimension(const LDOM_Element &valueN,
                              const std::shared_ptr<OCXContext> &ctx);

  /**
   * Parse a string of values, separated by spaces, under consideration of
   * multiplicities of theses values.
   *
   * @param value the string to parse
   * @param numKnots the number of total knot values (including multiplicities)
   * @return a struct containing the knot values and their corresponding
   * multiplicities
   */
  static KnotMults ParseKnotVector(std::string_view knotVectorS, int numKnots);

  /**
   * Read the poles and weights of a NURBS curve
   *
   * @param controlPtListN the control point list element
   * @param numCtrlPoints the number of control points
   * @param id the id of the curve
   * @param ctx the context used to lookup scaling factor
   * @return a struct containing the poles and weights of the curve
   */
  static PolesWeightsCurve ParseControlPointsCurve(
      LDOM_Element const &controlPtListN, int const &numCtrlPoints,
      std::string const &id, std::shared_ptr<OCXContext> const &ctx);

  /**
   * Read the poles and weights of a NURBS surface
   *
   * @param controlPtListN the control point list element
   * @param uNumCtrlPoints the number of control points in u direction
   * @param vNumCtrlPoints the number of control points in v direction
   * @param id the id of the surface
   * @param ctx the context used to lookup scaling factor
   * @return a struct containing the poles and weights of the surface
   */
  static PolesWeightsSurface ParseControlPointsSurface(
      LDOM_Element const &controlPtListN, int const &uNumCtrlPoints,
      int const &vNumCtrlPoints, std::string const &id,
      std::shared_ptr<OCXContext> const &ctx);
};

#endif  // OCX_INCLUDE_OCX_OCX_HELPER_H_
