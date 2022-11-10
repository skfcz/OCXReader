//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCX_INCLUDE_OCX_OCX_CAF_CONTROL_READER_H_
#define OCX_INCLUDE_OCX_OCX_CAF_CONTROL_READER_H_

#include <IFSelect_ReturnStatus.hxx>
#include <Resource_FormatType.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
#include <XmlObjMgt_Element.hxx>
#include <map>
#include <memory>
#include <string>

#include "ocx/internal/ocx-context.h"

class XSControl_WorkSession;

class TDocStd_Document;

class TCollection_AsciiString;

class TDF_Label;

/**
 * Provides a tool to read OCX file and put it into
 * OCAF document. Besides transfer of shapes (including
 * assemblies) supports also part names and properties.
 */
class OCXCAFControl_Reader {
 public:
  /**
   * Creates a reader with an empty model
   */
  Standard_EXPORT OCXCAFControl_Reader();

  /**
   * Destructor.
   */
  Standard_EXPORT ~OCXCAFControl_Reader();

  /**
   * Translate OCX file given by filename into the document
   * Return True if succeeded, and False in case of fail
   * @param filename the file to read
   * @return true if result is usable for Transfer
   */
  Standard_EXPORT Standard_Boolean ReadFile(Standard_CString filename);

  /**
   * Transfer the parsed model into OCAFS
   * @param doc the target model
   * @param theProgress progress
   * @return true if result could be used.
   */
  Standard_EXPORT Standard_Boolean
  Transfer(Handle(TDocStd_Document) & doc,
           const Message_ProgressRange &theProgress = Message_ProgressRange());

  /*
   * Combination of ReadFile and Transfer
   */
  Standard_EXPORT Standard_Boolean
  Perform(Standard_CString filename, Handle(TDocStd_Document) & doc,
          const Message_ProgressRange &theProgress = Message_ProgressRange());

 private:
  std::shared_ptr<OCXContext> ctx;
  
  LDOM_Element ocxDocEL;
  std::string nsPrefix;

  void ParseCoordinateSystem(LDOM_Element &coosysN);

  TopoDS_Shape ParseUnboundGeometry(LDOM_Element &unboundedGeometryN);
};

#endif  // OCX_INCLUDE_OCX_OCX_CAF_CONTROL_READER_H_
