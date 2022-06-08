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
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>


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

    // Some config stuff
    static inline bool CreatePanelContours=true;
    static inline bool CreatePanelSurfaces=true;
    static inline bool CreateReferenceSurfaces=true;


    void SetOCAFDoc(opencascade::handle<TDocStd_Document> &handle);
    opencascade::handle<TDocStd_Document> GetOCAFDoc();
    opencascade::handle<XCAFDoc_ShapeTool> GetOCAFShapeTool();
    opencascade::handle<XCAFDoc_ColorTool> GetOCAFColorTool();


private:
    std::map<std::string, TopoDS_Face> guid2refplane;

    std::map<std::string, double> unit2factor;
    LDOM_Element ocxDocN;
    std::string nsPrefix;
    LDOMString ocxGUIDRef;
    LDOMString ocxGUID;

    opencascade::handle<TDocStd_Document> ocafDoc;
    opencascade::handle<XCAFDoc_ShapeTool> ocafShapeTool;
    opencascade::handle<XCAFDoc_ColorTool> ocafColorTool;

};


#endif //OCXREADER_OCXCONTEXT_H
