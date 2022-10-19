/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#include "OCXCurveReader.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include "OCXHelper.h"

OCXCurveReader::OCXCurveReader(OCXContext *ctx) {
    this->ctx = ctx;
}

TopoDS_Wire OCXCurveReader::ReadCurve(LDOM_Element &curveRootN) {
    BRepBuilderAPI_MakeWire makeWire = BRepBuilderAPI_MakeWire();
    LDOM_Node childN = curveRootN.getFirstChild();

    while (childN != NULL) {
        const LDOM_Node::NodeType nodeType = childN.getNodeType();
        if (nodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (nodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element curveN = (LDOM_Element &)childN;

            TopoDS_Shape edge = TopoDS_Shape();

            if ("Ellipse3D" == OCXHelper::GetLocalTagName(curveN)) {
                // we can have only one closed curve
                return ReadEllipse3D(curveN);
            } else if ("CircumCircle3D" == OCXHelper::GetLocalTagName(curveN)) {
                // we can have only one closed curve
                return ReadCircumCircle3D(curveN);
            } else if ("Circle3D" == OCXHelper::GetLocalTagName(curveN)) {
                // we can have only one closed curve
                return ReadCircle(curveN);
            } else if ("CircumArc3D" == OCXHelper::GetLocalTagName(curveN)) {
                edge = ReadCircumArc3D(curveN);
            } else if ("Line3D" == OCXHelper::GetLocalTagName(curveN)) {
                edge = ReadLine3D(curveN);
            } else if ("CompositeCurve3D" == OCXHelper::GetLocalTagName(curveN)) {
                edge = ReadCompositeCurve3D(curveN);
            } else if ("PolyLine3D" == OCXHelper::GetLocalTagName(curveN)) {
                edge = ReadPolyLine3D(curveN);
            } else if ("NURBS3D" == OCXHelper::GetLocalTagName(curveN)) {
                edge = ReadNURBS3D(curveN);
            } else {
                std::cerr << "found unknown curve type " << curveN.getTagName().GetString() << std::endl;
                continue;
            }

            if (edge.ShapeType() == TopAbs_WIRE) {
                // we find a closed curve and return it immediately
                return TopoDS::Wire(edge);
            } else if (edge.ShapeType() == TopAbs_EDGE) {
                // we combine it to a

                makeWire.Add(TopoDS::Edge(edge));
            }
        }
        childN = childN.getNextSibling();
    }
    return makeWire.Wire();
}

TopoDS_Wire OCXCurveReader::ReadEllipse3D(LDOM_Element &ellipseN) {
    std::string id = std::string(ellipseN.getAttribute("id").GetString());
    std::cout << "ReadEllipse3D " << id << std::endl;

    LDOM_Element centerN = OCXHelper::GetFirstChild(ellipseN, "Center");
    if (centerN.isNull()) {
        std::cout << "could not find Ellipse3D/Center in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element majorDiaN = OCXHelper::GetFirstChild(ellipseN, "MajorDiameter");
    if (majorDiaN.isNull()) {
        std::cout << "could not find Ellipse3D/MajorDiameter in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element minorDiaN = OCXHelper::GetFirstChild(ellipseN, "MinorDiameter");
    if (majorDiaN.isNull()) {
        std::cout << "could not find Ellipse3D/MinorDiameter in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element majorAxisN = OCXHelper::GetFirstChild(ellipseN, "MajorAxis");
    if (majorAxisN.isNull()) {
        std::cout << "could not find Ellipse3D/MajorAxis in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element minorAxisN = OCXHelper::GetFirstChild(ellipseN, "MinorAxis");
    if (minorAxisN.isNull()) {
        std::cout << "could not find Ellipse3D/MinorAxis in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }

    gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
    double dMajor = OCXHelper::ReadDimension(majorDiaN, ctx);
    double dMinor = OCXHelper::ReadDimension(minorDiaN, ctx);
    gp_Dir majorAxis = OCXHelper::ReadDirection(majorAxisN);
    gp_Dir minorAxis = OCXHelper::ReadDirection(minorAxisN);
    gp_Dir normal = majorAxis.Crossed(minorAxis);

    gp_Ax2 cosys = gp_Ax2(center, normal, majorAxis);
    cosys.SetDirection(majorAxis);

    Handle(Geom_Ellipse) ellipse = new Geom_Ellipse(cosys, dMajor / 2.0, dMinor / 2.0);
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(ellipse);
    return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire OCXCurveReader::ReadCircumCircle3D(LDOM_Element &circleN) {
    std::string id = std::string(circleN.getAttribute("id").GetString());
    std::cout << "ReadCircumCircle3D " << id << std::endl;

    LDOM_Element positionsN = OCXHelper::GetFirstChild(circleN, "Positions");
    if (positionsN.isNull()) {
        std::cout << "could not find CircumCircle/Positions with curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }

    LDOM_Node childN = positionsN.getFirstChild();

    gp_Pnt p0;
    gp_Pnt p1;
    gp_Pnt p2;

    int pointCnt = 0;

    while (childN != NULL) {
        const LDOM_Node::NodeType nodeType = childN.getNodeType();
        if (nodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (nodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element pointN = (LDOM_Element &)childN;

            if ("Point3D" == OCXHelper::GetLocalTagName(pointN)) {
                if (pointCnt == 0) {
                    p0 = OCXHelper::ReadPoint(pointN, ctx);
                } else if (pointCnt == 1) {
                    p1 = OCXHelper::ReadPoint(pointN, ctx);
                } else if (pointCnt == 2) {
                    p2 = OCXHelper::ReadPoint(pointN, ctx);
                } else {
                    std::cout << "found more that 3 points in  CircumCircle/Positions with curve id='" << id << "'" << std::endl;
                }
                pointCnt++;
            } else {
                std::cout << "found element " << pointN.getTagName().GetString() << " in  CircumCircle/Positions with curve id='" << id << "'" << std::endl;
            }
        }
        childN = childN.getNextSibling();
    }

    Handle(Geom_Circle) circle = GC_MakeCircle(p0, p1, p2);
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
    return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Wire OCXCurveReader::ReadCircle(LDOM_Element &circleN) {
    std::string id = std::string(circleN.getAttribute("id").GetString());
    std::cout << "ReadCircle " << id << std::endl;

    LDOM_Element centerN = OCXHelper::GetFirstChild(circleN, "Center");
    if (centerN.isNull()) {
        std::cout << "could not find Circle3D/Center in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element diaN = OCXHelper::GetFirstChild(circleN, "Diameter");
    if (diaN.isNull()) {
        std::cout << "could not find Circle3D/Diameter in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }
    LDOM_Element normalN = OCXHelper::GetFirstChild(circleN, "Normal");
    if (normalN.isNull()) {
        std::cout << "could not find Circle3D/Normal in curve id='" << id << "'" << std::endl;
        return TopoDS_Wire();
    }

    gp_Pnt center = OCXHelper::ReadPoint(centerN, ctx);
    double diameter = OCXHelper::ReadDimension(diaN, ctx);
    gp_Dir normal = OCXHelper::ReadDirection(normalN);

    gp_Ax2 cosys = gp_Ax2(center, normal);

    Handle(Geom_Circle) circle = new Geom_Circle(cosys, diameter / 2.0);
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
    return BRepBuilderAPI_MakeWire(edge);
}

TopoDS_Edge OCXCurveReader::ReadCircumArc3D(LDOM_Element &circleN) {
    std::string id = std::string(circleN.getAttribute("id").GetString());
    std::cout << "ReadCircumArc3D " << id << std::endl;

    LDOM_Element startN = OCXHelper::GetFirstChild(circleN, "StartPoint");
    if (startN.isNull()) {
        std::cout << "could not find CircumArc3D/StartPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    LDOM_Element intN = OCXHelper::GetFirstChild(circleN, "IntermediatePoint");
    if (intN.isNull()) {
        std::cout << "could not find CircumArc3D/IntermediatePoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    LDOM_Element endN = OCXHelper::GetFirstChild(circleN, "EndPoint");
    if (endN.isNull()) {
        std::cout << "could not find CircumArc3D/EndPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }

    gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
    gp_Pnt intermediate = OCXHelper::ReadPoint(intN, ctx);
    gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

    Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(start, end, intermediate);
    return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Edge OCXCurveReader::ReadLine3D(LDOM_Element &lineN) {
    std::string id = std::string(lineN.getAttribute("id").GetString());
    std::cout << "ReadLine3D " << id << std::endl;

    LDOM_Element startN = OCXHelper::GetFirstChild(lineN, "StartPoint");
    if (startN.isNull()) {
        std::cout << "could not find Line3D/StartPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }

    LDOM_Element endN = OCXHelper::GetFirstChild(lineN, "EndPoint");
    if (endN.isNull()) {
        std::cout << "could not find Line3D/EndPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }

    gp_Pnt start = OCXHelper::ReadPoint(startN, ctx);
    gp_Pnt end = OCXHelper::ReadPoint(endN, ctx);

    Handle(Geom_TrimmedCurve) arc = GC_MakeSegment(start, end);
    return BRepBuilderAPI_MakeEdge(arc);
}

TopoDS_Shape OCXCurveReader::ReadCompositeCurve3D(LDOM_Element &curveN) {
    std::cerr << "ReadCompositeCurve3D is not implemented yet" << std::endl;

    TopoDS_Shape wire = TopoDS_Shape();

    if ("Line3D" == OCXHelper::GetLocalTagName(curveN)) {
        wire = ReadLine3D(curveN);
    } else if ("CircumArc3D" == OCXHelper::GetLocalTagName(curveN)) {
        wire = ReadCircumArc3D(curveN);
    } else if ("NURBS3D" == OCXHelper::GetLocalTagName(curveN)) {
        wire = ReadNURBS3D(curveN);
    } else if ("PolyLine3D" == OCXHelper::GetLocalTagName(curveN)) {
        wire = ReadPolyLine3D(curveN);
    } else {
        std::cerr << "found unknown curve type in CompositeCurve3D " << curveN.getTagName().GetString()
                  << std::endl;
    }
    return wire;
}

TopoDS_Shape OCXCurveReader::ReadPolyLine3D(LDOM_Element &curveN) {
    std::cerr << "ReadPolyLine3D is not implemented yet" << std::endl;
    return TopoDS_Shape();
}

TopoDS_Shape OCXCurveReader::ReadNURBS3D(LDOM_Element &nurbs3DN) {
    std::string id = std::string(nurbs3DN.getAttribute("id").GetString());

    std::cout << "ReadNURBS3D " << id << std::endl;

    LDOM_Element propsN = OCXHelper::GetFirstChild(nurbs3DN, "NURBSproperties");
    if (propsN.isNull()) {
        std::cout << "could not find NURBS3D/NURBSProperties in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    int degree;
    propsN.getAttribute("degree").GetInteger(degree);
    int numCtrlPts;
    propsN.getAttribute("numCtrlPts").GetInteger(numCtrlPts);
    int numKnots;
    propsN.getAttribute("numKnots").GetInteger(numKnots);
    std::string form = std::string(propsN.getAttribute("form").GetString());
    bool isRational = std::string(propsN.getAttribute("isRational").GetString()) == "true";

    // std::cout << "degree " << degree << ", #ctr " << numCtrlPts << ", #knots " << numKnots << ", form '" << form <<
    //           (isRational ? "', rational" : "', irrational") << std::endl;

    // The number of knots is always equals to the number of control points + curve degree + one
    // == number of control points + curve degree

    LDOM_Element knotVectorN = OCXHelper::GetFirstChild(nurbs3DN, "KnotVector");
    if (knotVectorN.isNull()) {
        std::cout << "could not find NURBS3D/KnotVector in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    std::string knotVectorS = std::string(knotVectorN.getAttribute("value").GetString());
    // std::cout << "knots[" << knotVectorS << "]" << std::endl;
    std::vector<std::string> out;
    OCXHelper::TokenizeBySpace(knotVectorS, out);

    if (numKnots != static_cast<int>(out.size())) {
        std::cerr << "failed to parse curve " << id << ", expected #" << numKnots << " knot values, but parsing [" << knotVectorS << "] resulted in #" << out.size() << " values"
                  << std::endl;
        return TopoDS_Edge();
    }
    // https://github.com/tpaviot/pythonocc-core/issues/706
    // Typically you'll have in textbooks a knot vector like:     [0 0 0 0.3 0.5 0.7 1 1 1]
    // In OCCT, you have instead:
    //    Knots: [0 0.3 0.5 0.7 1]
    //    Mults [3 1 1 1 3]

    std::vector<double> knots0;
    std::vector<int> mults0;
    double lastKnot = -1;
    double currentMult = -1;
    for (int i = 0; i < out.size(); i++) {
        double currentKnot = std::stod(out[i]);
        if (i == 0) {
            lastKnot = currentKnot;
            currentMult = 1;
            continue;
        }
        if (abs(currentKnot - lastKnot) < 1e-3) {
            currentMult++;
        } else {
            // knot value changed from lastKnot to currentKnot
            knots0.push_back(lastKnot);
            mults0.push_back(currentMult);
            lastKnot = currentKnot;
            currentMult = 1;
        }
    }
    knots0.push_back(lastKnot);
    mults0.push_back(currentMult);

    // now we know the size of the knot and multiplicities vector and
    //  could create the OCC type
    TColStd_Array1OfReal knots(1, knots0.size());
    TColStd_Array1OfInteger mults(1, knots0.size());
    for (int i = 0; i < knots0.size(); i++) {
        // std::cout << i << ", knot " << knots0[i] << ", mult " << mults0[i] << std::endl;
        knots.SetValue(i + 1, knots0[i]);
        mults.SetValue(i + 1, mults0[i]);
    }

    LDOM_Element controlPtListN = OCXHelper::GetFirstChild(nurbs3DN, "ControlPtList");
    if (controlPtListN.isNull()) {
        std::cout << "could not find NURBS3D/ControlPtList in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }
    // names of the elements we are looking for
    LDOMString controlPointT = LDOMString((ctx->Prefix() + ":ControlPoint").c_str());
    LDOMString point3dT = LDOMString((ctx->Prefix() + ":Point3D").c_str());
    LDOMString xT = LDOMString((ctx->Prefix() + ":X").c_str());
    LDOMString yT = LDOMString((ctx->Prefix() + ":Y").c_str());
    LDOMString zT = LDOMString((ctx->Prefix() + ":Z").c_str());

    LDOM_Element controlPointN = controlPtListN.GetChildByTagName(controlPointT);
    if (controlPointN.isNull()) {
        std::cout << "could not find NURBS3D/ControlPtList/ControlPoint in curve id='" << id << "'" << std::endl;
        return TopoDS_Edge();
    }

    TColgp_Array1OfPnt poles(1, numCtrlPts);
    TColStd_Array1OfReal weights(1, numCtrlPts);

    int i = 0;
    while (!controlPointN.isNull()) {
        double weight = 1;
        OCXHelper::GetDoubleAttribute(controlPointN, "weight", weight);

        LDOM_Element pointN = controlPointN.GetChildByTagName(point3dT);
        LDOM_Element xN = pointN.GetChildByTagName(xT);
        LDOM_Element yN = pointN.GetChildByTagName(yT);
        LDOM_Element zN = pointN.GetChildByTagName(zT);

        double x;
        OCXHelper::GetDoubleAttribute(xN, "numericvalue", x);
        std::string xUnit = std::string(xN.getAttribute("unit").GetString());
        x *= ctx->LoopupFactor(xUnit);

        double y;
        OCXHelper::GetDoubleAttribute(yN, "numericvalue", y);
        std::string yUnit = std::string(yN.getAttribute("unit").GetString());
        y *= ctx->LoopupFactor(yUnit);

        double z;
        OCXHelper::GetDoubleAttribute(zN, "numericvalue", z);
        std::string zUnit = std::string(zN.getAttribute("unit").GetString());
        z *= ctx->LoopupFactor(zUnit);

        //        std::cout << controlPointN.getTagName().GetString() <<  " #" << i << " weight " << weight
        //            << ", [" << x << ", " << y << "," << z << "] " << xUnit  << std::endl;

        poles.SetValue(i + 1, gp_Pnt(x, y, z));
        weights.SetValue(i + 1, weight);

        controlPointN = controlPointN.GetSiblingByTagName();
        i++;
    }

    Handle(Geom_BSplineCurve) curve = new Geom_BSplineCurve(poles, weights, knots, mults, degree);

    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(curve);

    if (curve->IsClosed()) {
        return BRepBuilderAPI_MakeWire(edge);
    }
    return edge;
}
