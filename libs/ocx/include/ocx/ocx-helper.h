/***************************************************************************
 *   Created on: 31 May 2022                                               *
 ***************************************************************************
 *   Copyright (c) 2022, Carsten Zerbst (carsten.zerbst@groy-groy.de)      *
 *   Copyright (c) 2022, Paul Buechner                                     *
 *                                                                         *
 *   This file is part of the OCXReader library.                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   version 2.1 as published by the Free Software Foundation.             *
 *                                                                         *
 ***************************************************************************/

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_HELPER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_HELPER_H_

#include <Bnd_Box.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <LDOM_Element.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "ocx/internal/ocx-log.h"
#include "ocx/ocx-context.h"

namespace ocx::helper {

struct KnotMults {
  TColStd_Array1OfReal knots;
  TColStd_Array1OfInteger mults;
  bool IsNull = false;
};

struct PolesWeightsCurve {
  TColgp_Array1OfPnt poles;
  TColStd_Array1OfReal weights;
  bool IsNull = false;

  explicit PolesWeightsCurve(int const &num) : poles(1, num), weights(1, num) {}
};

struct PolesWeightsSurface {
  TColgp_Array2OfPnt poles;
  TColStd_Array2OfReal weights;
  bool IsNull = false;

  PolesWeightsSurface(int const &uNum, int const &vNum)
      : poles(1, uNum, 1, vNum), weights(1, uNum, 1, vNum) {}
};

struct OCXMeta {
  char const *name, *id, *localRef, *guid;

  std::string refType;

  OCXMeta(char const *name, char const *id, char const *localRef,
          char const *guid, std::string refType)
      : name(name),
        id(id),
        localRef(localRef),
        guid(guid),
        refType(std::move(refType)){};
};

/**
 * Get the meta information of a given DOM element
 *
 * @param element the element to get the meta information from
 * @return struct holding the meta information or nullptr if LDOM_Element is
 * null
 */
std::unique_ptr<OCXMeta> GetOCXMeta(LDOM_Element const &element);

std::string GetLocalTagName(LDOM_Element const &elem, bool keepPrefix = false);

std::string GetLocalAttrName(LDOM_Node const &node);

std::string GetAttrValue(LDOM_Element const &element, std::string const &name);

std::string GetChildTagName(LDOM_Element const &parent,
                            bool keepPrefix = false);

LDOM_Element GetFirstChild(LDOM_Element const &parent,
                           std::string_view localName);

/**
 * @brief Get the integer attribute value from an attribute. If the attribute is
 * missing the value is not touched
 *
 * @param elem the element containing the attribute
 * @param attrName the attributes name
 * @param value the variable to put the value
 */
void GetIntegerAttribute(LDOM_Element const &elem, std::string const &attrName,
                         Standard_Integer &value);

/**
 * @brief Get the double attribute value from an attribute. Does not care if
 * this is more an integer '50' or double '50.0'. If the attribute is missing
 * the value is not touched
 *
 * @param elem the element containing the attribute
 * @param attrName the attributes name
 * @param value the variable to put the value
 */
void GetDoubleAttribute(LDOM_Element const &elem, std::string const &attrName,
                        Standard_Real &value);

gp_Pnt ReadPoint(LDOM_Element const &pointN);

gp_Dir ReadDirection(LDOM_Element const &dirN);

gp_Trsf ReadTransformation(LDOM_Element transfEle);

double ReadDimension(LDOM_Element const &valueN);

/**
 * @brief Parse a string of values, separated by spaces, under consideration
 * of multiplicities of theses values.
 *
 * @param value the string to parse
 * @param numKnots the number of total knot values (including multiplicities)
 * @return a struct containing the knot values and their corresponding
 * multiplicities
 */
KnotMults ParseKnotVector(std::string_view knotVectorS, int const &numKnots);

/**
 * Read the poles and weights of a NURBS curve
 *
 * @param controlPtListN the control point list element
 * @param numCtrlPoints the number of control points
 * @return a struct containing the poles and weights of the curve
 */
PolesWeightsCurve ParseControlPointsCurve(LDOM_Element const &controlPtListN,
                                          int const &numCtrlPoints);

/**
 * Read the poles and weights of a NURBS surface
 *
 * @param controlPtListN the control point list element
 * @param uNumCtrlPoints the number of control points in u direction
 * @param vNumCtrlPoints the number of control points in v direction
 * @return a struct containing the poles and weights of the surface
 */
PolesWeightsSurface ParseControlPointsSurface(
    LDOM_Element const &controlPtListN, int const &uNumCtrlPoints,
    int const &vNumCtrlPoints);

/**
 * @brief Take a TopoDS_Face or TopoDS_Shell and cut it by the given
 * TopoDS_Wire
 *
 * @param shape the shape to cut (TopoDS_Face or TopoDS_Shell)
 * @param wire the wire to cut with
 * @return the cut shape or an empty shape if the cut failed
 */
TopoDS_Shape LimitShapeByWire(TopoDS_Shape const &shape,
                              TopoDS_Wire const &wire, std::string_view id,
                              std::string_view guid);

/**
 * Compute the 3D intersection between a two surfaces
 * @returns nullopt if there is no intersection or the Algorithm fails, the 3D
 * curve else
 */
std::optional<TopoDS_Edge> Intersection(const GeomAdaptor_Surface &S1,
                                        const GeomAdaptor_Surface &S2);

/**
 * Compute the 3D intersection between a curve and a bounding box
 * @returns nullopt if there is no intersection or the Algorithm fails, a vector
 * containing the points of intersections else
 */
std::optional<TopoDS_Edge> CurveLimitByBoundingBox(
    const GeomAdaptor_Curve &curve, const Bnd_Box &box);

}  // namespace ocx::helper

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_HELPER_H_
