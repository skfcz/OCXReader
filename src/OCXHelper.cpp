//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "OCXHelper.h"
#include <stdio.h>
#include <string.h>
#include <vector>


std::string OCXHelper::GetLocalTagName(LDOM_Element &elem) {

    std::string tagName = std::string(elem.getTagName().GetString());
    int idx = tagName.find(':');
    if (idx != std::string::npos) {
        return tagName.substr(idx + 1);
    }
    return tagName;

}


std::string OCXHelper::GetLocalAttrName(LDOM_Node &elem) {
    std::string tagName = std::string(elem.getNodeName().GetString());
    int idx = tagName.find(':');
    if (idx != std::string::npos) {
        return tagName.substr(idx + 1);
    }
    return tagName;
}

LDOM_Element OCXHelper::GetFirstChild(LDOM_Element &parent, std::string localName) {

    //std::cout << "localName '" <<localName << "'" << std::endl;

    // Verify preconditions
    LDOM_Element aVoidElement;
    if (parent == NULL || localName.length() == 0) {
        return aVoidElement;
    }

    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = parent.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();

        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aNextElement = (LDOM_Element &) aChildNode;

            if (localName.compare(GetLocalTagName(aNextElement)) == 0) {
                return aNextElement;
            }
        }
        aChildNode = aChildNode.getNextSibling();
    }
    return aVoidElement;

}

void OCXHelper::TokenizeBySpace(const std::string &str, std::vector<std::string> &out) {

    char *token = strtok(const_cast<char *>(str.c_str()), " ");
    while (token != nullptr) {
        out.push_back(std::string(token));
        token = strtok(nullptr, " ");
    }

}

void OCXHelper::GetDoubleAttribute(LDOM_Element &elem, std::string attrName, Standard_Real &value) {

    LDOMString stringValue = elem.getAttribute(attrName.c_str());
    if ( stringValue.Type() == LDOMBasicString::StringType::LDOM_NULL) {
        return;
    }

    if (strlen(stringValue.GetString()) > 0) {
        double d = strtod(stringValue.GetString(), NULL);
        value = Standard_Real(d);
        return;
    }

    int i;
    stringValue.GetInteger(i);
    value = Standard_Real((double) i);

}

double OCXHelper::ReadDimension( LDOM_Element valueN, OCXContext * ctx) {
    double x;
    OCXHelper::GetDoubleAttribute(valueN, "numericvalue", x);
    std::string xUnit = std::string(valueN.getAttribute("unit").GetString());
    x *= ctx->LoopupFactor(xUnit);
    return x;
}

gp_Pnt OCXHelper::ReadPoint(LDOM_Element pointN, OCXContext *ctx) {

    LDOMString xT = LDOMString((ctx->Prefix() + ":X").c_str());
    LDOMString yT = LDOMString((ctx->Prefix() + ":Y").c_str());
    LDOMString zT = LDOMString((ctx->Prefix() + ":Z").c_str());

    LDOM_Element xN = pointN.GetChildByTagName(xT);
    LDOM_Element yN = pointN.GetChildByTagName(yT);
    LDOM_Element zN = pointN.GetChildByTagName(zT);

    // TODO: use ReadDimension
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


    return gp_Pnt(x, y, z);
}

gp_Dir OCXHelper::ReadDirection(LDOM_Element dirN) {

    double x;
    OCXHelper::GetDoubleAttribute(dirN, "x", x);

    double y;
    OCXHelper::GetDoubleAttribute(dirN, "y", y);

    double z;
    OCXHelper::GetDoubleAttribute(dirN, "z", z);

    return gp_Dir(x, y, z);
}

KnotMults OCXHelper::ParseKnotVector(std::string knotVectorS, int numKnots) {

    KnotMults kn = KnotMults();

    //std::cout << "knots[" << knotVectorS << "]" << std::endl;
    std::vector<std::string> out;
    OCXHelper::TokenizeBySpace(knotVectorS, out);

    if (numKnots != static_cast<int>(out.size())) {
        std::cerr << "failed to parse knotvector, expected #" << numKnots <<
                  " knot values, but parsing [" << knotVectorS << "] resulted in #" << out.size() << " values"
                  << std::endl;
        kn.IsNull=true;
        return kn;
    }
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
    kn.knots=knots;
    kn.mults=mults;


    for (int i = 0; i < knots0.size(); i++) {
        //std::cout << i << ", knot " << knots0[i] << ", mult " << mults0[i] << std::endl;
        knots.SetValue(i + 1, knots0[i]);
        mults.SetValue(i + 1, mults0[i]);
    }

    return kn;
}

PolesWeights OCXHelper::ParseControlPoints(LDOM_Element controlPtListN, int uNumCtrlPoints, int vNumCtrlPoints, std::string id) {

    PolesWeights  polesWeights = PolesWeights();



    TColgp_Array2OfPnt cpArray(1, uNumCtrlPoints, 1, vNumCtrlPoints);
    TColStd_Array1OfInteger uMultsArray(1, uNumCtrlPoints);
    TColStd_Array1OfInteger vMultsArray(1, vNumCtrlPoints);


    LDOM_Element controlPointN = controlPtListN.GetChildByTagName(controlPtListN);
    if (controlPointN.isNull()) {
        std::cerr<< "could not find NURBSSurface/ControlPtList/ControlPoint[0] in surface id='" << id << "'" << std::endl;
        polesWeights.IsNull = true;
        return polesWeights;
    }


    int i = 0;
    int u = 1;
    int v = 1;
    while (!controlPointN.isNull()) {

        double weight=1;
        OCXHelper::GetDoubleAttribute(controlPointN, "weight", weight);

        LDOM_Element pointN = GetFirstChild( controlPointN, "Point3D");
        if ( pointN.isNull()) {
                std::cerr<< "could not find NURBSSurface/ControlPtList/ControlPoint[" << i <<"]/Point3D in surface id='" << id << "'" << std::endl;
                polesWeights.IsNull = true;
                return polesWeights;
        }
        gp_Pnt point = ReadPoint(pointN);
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
        cpArray.SetValue(u,v, point);

        //        std::cout << controlPointN.getTagName().GetString() <<  " #" << i << " weight " << weight
        //            << ", [" << x << ", " << y << "," << z << "] " << xUnit  << std::endl;

        poles.SetValue(i + 1, gp_Pnt(x, y, z));
        weights.SetValue(i + 1, weight);

        controlPointN = controlPointN.GetSiblingByTagName();
        i++;
    }


    for ( int i = 1; i<= uNumCtrlPoints;i++) {
        for ( int j = 1; j<= vNumCtrlPoints;j++) {

        }
    }
    return PolesWeights();
}


