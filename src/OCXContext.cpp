//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include <UnitsAPI.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include "OCXContext.h"
#include "OCXHelper.h"

OCXContext::OCXContext(LDOM_Element &ocxDocN, std::string nsPrefix) {
    this->ocxDocN = ocxDocN;
    this->nsPrefix =nsPrefix;

    this->ocxGUIDRef = LDOMString( (nsPrefix + ":GUIDRef").c_str());
    this->ocxGUID = LDOMString( (nsPrefix + ":GUID").c_str());

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
    LDOM_Node aChildNode = unitsSetN.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element unitN = (LDOM_Element &) aChildNode;
            if ("Unit" == OCXHelper::GetLocalTagName(unitN)) {
                LDOM_NodeList attributes = unitN.GetAttributesList();

                std::string id;

                for( int i = 0; i < attributes.getLength();i++) {
                    LDOM_Node theAtt = attributes.item (i);

                    std::string name = OCXHelper::GetLocalAttrName( theAtt);

                    if ( "id" == name) {
                        id = std::string(theAtt.getNodeValue().GetString());
                    }
                }

                std::string symbol = OCXHelper::GetFirstChild( unitN, "UnitSymbol").getAttribute("type").GetString();
                //std::cout << "id  " << id<< ", symbol " << symbol <<  std::endl;

                if ( "m" == symbol) {
                    unit2factor[id] = 1;
                } else if ( "dm" == symbol) {
                    unit2factor[id] = 1/10.0;
                } else if ( "cm" == symbol) {
                    unit2factor[id] = 1/100.0;
                } else if ( "mm" == symbol) {
                    unit2factor[id] = 1/1000.0;
                }

            }
        }
        aChildNode = aChildNode.getNextSibling();
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

LDOMString OCXContext::OCXGUID() {
    return ocxGUID;
}


void OCXContext::RegisterSurface(std::string guid, TopoDS_Shape face) {
    guid2refplane[ guid]=face;
}

TopoDS_Shape OCXContext::LookupSurface(std::string guid) {
    // TODO: check if exist
    return guid2refplane[guid];
}

void OCXContext::SetOCAFDoc(opencascade::handle<TDocStd_Document> &handle) {
    ocafDoc = handle;
    ocafShapeTool = XCAFDoc_DocumentTool::ShapeTool( ocafDoc->Main() ); // Shape tool.
    ocafColorTool = XCAFDoc_DocumentTool::ColorTool( ocafDoc->Main() ); // Color tool.
}

opencascade::handle<TDocStd_Document> OCXContext::GetOCAFDoc() {
    return ocafDoc;
}

opencascade::handle<XCAFDoc_ShapeTool> OCXContext::GetOCAFShapeTool() {
    return ocafShapeTool;
}

opencascade::handle<XCAFDoc_ColorTool> OCXContext::GetOCAFColorTool() {
    return ocafColorTool;
}

