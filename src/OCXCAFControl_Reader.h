//
// Created by cz on 22.05.22.
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
private:

    LDOM_Element ocxDocEL;
    std::string nsPrefix;

    std::map<std::string, double> unit2factor;


    void PrepareUnits();
    TopoDS_Shape ParsePanels(LDOM_Element& vesselN, TDF_Label vesselL);
    TopoDS_Shape ParsePanel(LDOM_Element& pannelN, TDF_Label vesselL);
    TopoDS_Shape  ParseUnboundGeometry(LDOM_Element& unboundedGeometryN);

    TopoDS_Wire ParseCurve( LDOM_Element& curveN);
    TopoDS_Edge ParseNURBSCurve(LDOM_Element& element);

    double LoopupFactor( std::string unit);



};


#endif //OCXCAFCONTROL_READER_H
