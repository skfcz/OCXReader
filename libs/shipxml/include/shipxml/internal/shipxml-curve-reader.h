/***************************************************************************
 *   Created on: 09 Jan 2023                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CURVE_READER_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CURVE_READER_H_

#include <Geom_BSplineCurve.hxx>
#include <LDOM_Element.hxx>
#include <gp_Dir.hxx>

#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-enums.h"

namespace shipxml {

[[nodiscard]] shipxml::AMCurve ReadCurve(LDOM_Element const &curveRootN,
                                         MajorPlane const &mp,
                                         gp_Dir const &normal);

[[nodiscard]] std::vector<ArcSegment> ReadCompositeCurve3D(
    LDOM_Element const &crvEL, AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadEllipse3D(LDOM_Element const &crvEL,
                                                    AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadCircumCircle3D(
    LDOM_Element const &crvEL, AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadCircle3D(LDOM_Element const &crvEL,
                                                   AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadCircumArc3D(LDOM_Element const &crvEL,
                                                      AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadLine3D(LDOM_Element const &crvEL,
                                                 AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadPolyLine3D(LDOM_Element const &crvEL,
                                                     AMCurve const &curve);

[[nodiscard]] std::vector<ArcSegment> ReadNURBS3D(LDOM_Element const &crvEL,
                                                  AMCurve const &curve);

//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------

void Refine(opencascade::handle<Geom_BSplineCurve> const &handle1,
            double startU, double endU, std::vector<gp_Pnt> &points);

[[nodiscard]] std::vector<ArcSegment> CreateArcSegments(
    std::vector<gp_Pnt> &points, int startRangeIdx, int endRangeIdx);

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_CURVE_READER_H_
