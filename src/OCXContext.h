//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADER_OCXCONTEXT_H
#define OCXREADER_OCXCONTEXT_H

#include <string>
#include <map>
#include <LDOM_Element.hxx>
#include <TopoDS_Face.hxx>

class OCXContext {

public:
    OCXContext(LDOM_Element & ocxDocN, std::string nsPrefix);

    std::string GetPrefix();

    void PrepareUnits();
    double LoopupFactor( std::string unit);

    void RegisterSurface(std::string guid, TopoDS_Face face);
    TopoDS_Face LookupSurface(std::string guid);

    LDOMString OCXGUIDRef();
    LDOMString OCXGUID();



private:
    std::map<std::string, TopoDS_Face> guid2refplane;

    std::map<std::string, double> unit2factor;
    LDOM_Element ocxDocN;
    std::string nsPrefix;
    LDOMString ocxGUIDRef;
    LDOMString ocxGUID;

};


#endif //OCXREADER_OCXCONTEXT_H
