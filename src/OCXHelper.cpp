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
    if (strlen(stringValue.GetString()) > 0) {
        double d = strtod(stringValue.GetString(), NULL);
        value = Standard_Real(d);
        return;
    }

    int i;
    stringValue.GetInteger(i);
    value = Standard_Real((double) i);

}

gp_Pnt OCXHelper::ReadPoint(LDOM_Element pointN, OCXContext *ctx) {

    LDOMString xT = LDOMString((ctx->GetPrefix() + ":X").c_str());
    LDOMString yT = LDOMString((ctx->GetPrefix() + ":Y").c_str());
    LDOMString zT = LDOMString((ctx->GetPrefix() + ":Z").c_str());

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
