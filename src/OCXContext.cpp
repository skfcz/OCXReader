//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include <UnitsAPI.hxx>
#include "OCXContext.h"
#include "OCXHelper.h"

OCXContext::OCXContext(LDOM_Element &ocxDocN, std::string nsPrefix) {
    this->ocxDocN = ocxDocN;
    this->nsPrefix =nsPrefix;

    this->ocxGUIDRef = LDOMString( (nsPrefix + ":GUIDRef").c_str());
}


void OCXContext::PrepareUnits() {

    // we use everything based on m
    UnitsAPI::SetLocalSystem(UnitsAPI_SI);

    // set some default values
    unit2factor["Um"] = 1;
    unit2factor["Udm"] = 0.1;
    unit2factor["Ucm"] = 0.01;
    unit2factor["Umm"] = 0.001;

    LDOM_Element unitsMLN = OCXHelper::GetFirstChild(ocxDocN, "UnitsML");
    if (unitsMLN.isNull()) {
        std::cout << "could not find UnitsML element" << std::endl;
        return;
    }

    LDOM_Node unitsSetN = OCXHelper::GetFirstChild(unitsMLN, "UnitSet");
    if (unitsSetN.isNull()) {
        std::cout << "could not find UnitsML/UnitSet element" << std::endl;
        return;
    }
    // TODO: really use the definitions in UnitsSet/Unit


}

double OCXContext::LoopupFactor(std::string unit) {
    auto res = unit2factor.find(unit);
    if (res != unit2factor.end()) {
        return res->second;
    }
    std::cout << "could not find factor for unit '" << unit << "', use 1.0" << std::endl;
    return 1;
}

std::string OCXContext::GetPrefix() {
    return nsPrefix;
}

LDOMString OCXContext::OCXGUIDRef() {
    return ocxGUIDRef;
}

void OCXContext::RegisterRefplane(std::string guid, TopoDS_Face face) {
    guid2refplane[ guid]=face;


}
