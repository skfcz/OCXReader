// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_OCX_HELPER_H_
#define OCX_INCLUDE_OCX_OCX_HELPER_H_

#include <LDOM_Element.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <memory>
#include <vector>

namespace shipxml {

class ShipXMLHelper {
 public:
  static std::string GetLocalTagName(LDOM_Element const &elem,
                                     bool keepPrefix = false);

  static std::string GetLocalAttrName(LDOM_Node const &node);

  static std::string GetChildTagName(LDOM_Element const &parent,
                                     bool keepPrefix = false);

  static LDOM_Element GetFirstChild(LDOM_Element const &parent,
                                    std::string_view localName);

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
                       std::string const &delimiters = " ",
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
  static void GetDoubleAttribute(LDOM_Element const &elem,
                                 std::string const &attrName,
                                 Standard_Real &value);

//  static gp_Pnt ReadPoint(LDOM_Element const &pointN,
//                          std::shared_ptr<OCXContext> const &ctx);

  static gp_Dir ReadDirection(LDOM_Element const &dirN);

//  static double ReadDimension(LDOM_Element const &valueN,
//                              std::shared_ptr<OCXContext> const &ctx);

  static const char *ReadGUID(LDOM_Element const &element);
  static std::string GetAttribute(LDOM_Element element, std::string string);
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_HELPER_H_
