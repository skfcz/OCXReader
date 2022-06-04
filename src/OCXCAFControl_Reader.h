//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXCAFCONTROL_READER_H
#define OCXCAFCONTROL_READER_H

#include <Resource_FormatType.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Wire.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <XmlObjMgt_Element.hxx>
#include <string>
#include <map>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include "OCXContext.h"

class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;
class TDF_Label;




//! Provides a tool to read OCX file and put it into
//! OCAF document. Besides transfer of shapes (including
//! assemblies) supports also part names and properties.
//!
class OCXCAFControl_Reader {
public:
    //! Creates a reader with an empty model
    Standard_EXPORT OCXCAFControl_Reader();

    //! Destructor.
    Standard_EXPORT virtual ~OCXCAFControl_Reader();


    //! Translate OCX file given by filename into the document
    //! Return True if succeeded, and False in case of fail
    Standard_EXPORT Standard_Boolean ReadFile (const Standard_CString filename);


    Standard_EXPORT Standard_Boolean Transfer (Handle(TDocStd_Document) &doc,
                                               const Message_ProgressRange& theProgress = Message_ProgressRange());

    Standard_EXPORT  Standard_Boolean Perform (const Standard_CString filename,
                                                     Handle(TDocStd_Document) &doc,
                                               const Message_ProgressRange& theProgress = Message_ProgressRange());
    OCXContext *  Ctx();

private:

    OCXContext * ctx;
    LDOM_Element ocxDocEL    ;
    std::string nsPrefix;


    void ParseCoordinateSystem(LDOM_Element& coosysN);
    TopoDS_Shape ParsePanels(LDOM_Element& vesselN, TDF_Label vesselL);
    TopoDS_Shape ParsePanel(LDOM_Element& pannelN, TDF_Label vesselL);
    TopoDS_Shape  ParseUnboundGeometry(LDOM_Element& unboundedGeometryN);

    TopoDS_Wire ParseCurve( LDOM_Element& curveN);
    TopoDS_Edge ParseNURBSCurve(LDOM_Element& element);


};


#endif //OCXCAFCONTROL_READER_H
