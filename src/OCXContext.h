/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXCONTEXT_H
#define OCXREADER_OCXCONTEXT_H

#include <LDOM_Element.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <map>
#include <string>

class OCXContext {
   public:
    OCXContext(LDOM_Element &ocxDocN, const std::string nsPrefix);

    void PrepareUnits();
    double LoopupFactor(std::string unit);

    void RegisterSurface(std::string guid, TopoDS_Shape shell);
    TopoDS_Shape LookupSurface(std::string guid);

    std::string Prefix();
    LDOMString OCXGUIDRef();
    LDOMString OCXGUID();

    // Some config stuff
    static inline bool CreatePanelContours = true;
    static inline bool CreatePanelSurfaces = true;
    static inline bool CreateReferenceSurfaces = true;
    static inline bool CreatePlateSurfaces = true;
    static inline bool CreateStiffenerTraces = true;

    void OCAFDoc(opencascade::handle<TDocStd_Document> &handle);
    opencascade::handle<TDocStd_Document> OCAFDoc();
    opencascade::handle<XCAFDoc_ShapeTool> OCAFShapeTool();
    opencascade::handle<XCAFDoc_ColorTool> OCAFColorTool();

   private:
    std::map<std::string, TopoDS_Shape> guid2refplane;
    std::map<std::string, double> unit2factor;
    LDOM_Element ocxDocN;
    std::string nsPrefix;
    LDOMString ocxGUIDRef;
    LDOMString ocxGUID;

    opencascade::handle<TDocStd_Document> ocafDoc;
    opencascade::handle<XCAFDoc_ShapeTool> ocafShapeTool;
    opencascade::handle<XCAFDoc_ColorTool> ocafColorTool;
};

#endif  // OCXREADER_OCXCONTEXT_H
