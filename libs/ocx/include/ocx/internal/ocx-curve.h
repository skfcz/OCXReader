/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_CURVE_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_CURVE_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <memory>

#include "ocx/ocx-context.h"

/**
 * This namespace is used to read OCX XML for curves and create OpenCascade
 * TopoDS_Wires.
 */
namespace ocx::reader::shared::curve {

/**
 * Base method managing the reading of various curve types, e.g.
 * Ellipse3D, CircumCircle3D, Circle3D, CircumArc3D, Line3D, CompositeCurve3D,
 * PolyLine3D, NURBS3D
 *
 * @param curveRootN the curve root element
 * @return the created TopoDS_Wire
 */
[[nodiscard]] TopoDS_Wire ReadCurve(LDOM_Element const &curveRootN);

namespace {  // anonymous namespace

[[nodiscard]] TopoDS_Shape ReadCompositeCurve3D(LDOM_Element const &curveColN);

[[nodiscard]] TopoDS_Wire ReadEllipse3D(LDOM_Element const &ellipseN);

[[nodiscard]] TopoDS_Wire ReadCircumCircle3D(LDOM_Element const &circleN);

[[nodiscard]] TopoDS_Wire ReadCircle3D(LDOM_Element const &circleN);

[[nodiscard]] TopoDS_Edge ReadCircumArc3D(LDOM_Element const &curveN);

[[nodiscard]] TopoDS_Edge ReadLine3D(LDOM_Element const &lineN);

[[nodiscard]] TopoDS_Shape ReadPolyLine3D(LDOM_Element const &curveN);

[[nodiscard]] TopoDS_Shape ReadNURBS3D(LDOM_Element const &nurbs3DN);

}  // namespace

}  // namespace ocx::reader::shared::curve

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_CURVE_READER_H_
