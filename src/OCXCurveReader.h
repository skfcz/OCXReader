//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//
#ifndef OCXREADER_OCXCURVEREADER_H
#define OCXREADER_OCXCURVEREADER_H


#include <TopoDS_Wire.hxx>
#include "OCXContext.h"

/**
 * This class is used to read OCX XML for curves and create OpenCascade TopoDS_Wires.
 */
class OCXCurveReader {
public:
    /**
     * Create a new curve reader
     * @param ctx the context used lookup scaling factor
     */
    OCXCurveReader(OCXContext *ctx);

    /**
     * Read a curve from the element,e.g. an FaceBoundaryCurve or OuterContur node
     * @param curveParentN the curve parent element element containing the curve(s)
     * @return the created TopoDS_Wire or TopoDS_Edge
     */
    TopoDS_Wire  ReadCurve( LDOM_Element & curveParentN );

private:
    OCXContext * ctx;


    TopoDS_Wire  ReadEllipse3D(LDOM_Element & curveN);
    TopoDS_Wire  ReadCircumCircle3D(LDOM_Element & circleN);
    TopoDS_Wire  ReadCircle(LDOM_Element & circleN);
    TopoDS_Edge  ReadCircumArc3D(LDOM_Element & curveN);
    TopoDS_Edge  ReadLine3D(LDOM_Element & lineN);
    TopoDS_Shape  ReadCompositeCurve3D(LDOM_Element & curveN);
    TopoDS_Shape  ReadPolyLine3D(LDOM_Element & curveN);
    TopoDS_Shape  ReadNURBS3D(LDOM_Element & curveN);
};


#endif //OCXREADER_OCXCURVEREADER_H
