/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXHELPER_H
#define OCXREADER_OCXHELPER_H

#include <LDOM_Element.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <vector>

#include "OCXContext.h"

struct KnotMults {
  TColStd_Array1OfReal knots;
  TColStd_Array1OfInteger mults;
  bool IsNull;
};

struct PolesWeights {
  TColgp_Array2OfPnt poles;
  TColStd_Array1OfReal weights;
  bool IsNull;
};

class OCXHelper {
 public:
  static std::string GetLocalTagName(LDOM_Element &elem);

  static std::string GetLocalAttrName(LDOM_Node &node);

  static LDOM_Element GetFirstChild(LDOM_Element &parent,
                                    std::string localName);

  /**
   * Split a string by given delimiter and return a Container of type T
   * @see https://stackoverflow.com/a/1493195/13279727
   *
   * @tparam ContainerT Container type
   * @param str String to split
   * @param tokens Container to store the result
   * @param delimiters Delimiter used to split the string (default: " ")
   * @param trimEmpty Trim empty tokens (default: false)
   */
  template <class ContainerT>
  static void Tokenize(std::string_view str, ContainerT &tokens,
                       const std::string &delimiters = " ",
                       bool trimEmpty = false);

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

  static gp_Pnt ReadPoint(const LDOM_Element &pointN, OCXContext *ctx);

  static gp_Dir ReadDirection(const LDOM_Element &dirN);

  static double ReadDimension(const LDOM_Element &valueN, OCXContext *ctx);

  /**
   * Parse a string of values, separated by spaces, under consideration of
   * multiplicities of theses values.
   *
   * @param value the string to parse
   * @param numKnots the number of total knot values (including multiplicities)
   * @return a struct containing the knot values and their corresponding multiplicities
   */
  static KnotMults ParseKnotVector(const std::string &value, int numKnots);

  static PolesWeights ParseControlPoints(const LDOM_Element &controlPtListN,
                                         int uNumCtrlPoints, int vNumCtrlPoints,
                                         const std::string &id,
                                         OCXContext *ctx);
};

#endif  // OCXREADER_OCXHELPER_H
