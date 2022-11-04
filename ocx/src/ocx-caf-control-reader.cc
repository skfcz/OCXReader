//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/ocx-caf-control-reader.h"

#include <BRep_Builder.hxx>
#include <LDOMParser.hxx>
#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <OSD_FileSystem.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Writer.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <UnitsAPI.hxx>
#include <memory>
#include <string>

#include "ocx/internal/ocx-coordinate-system-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-panel-reader.h"
#include "ocx/internal/ocx-reference-surfaces-reader.h"
#include "ocx/internal/ocx-util.h"

OCXCAFControl_Reader::OCXCAFControl_Reader() = default;

OCXCAFControl_Reader::~OCXCAFControl_Reader() = default;

Standard_Boolean OCXCAFControl_Reader::ReadFile(Standard_CString filename) {
  // Load the OCX Document as DOM
  const Handle(OSD_FileSystem) &aFileSystem =
      OSD_FileSystem::DefaultFileSystem();
  opencascade::std::shared_ptr<std::istream> aFileStream =
      aFileSystem->OpenIStream(filename, std::ios::in);

  if (aFileStream == nullptr || !aFileStream->good()) {
    std::cerr << "cannot open '" << filename << "' for reading" << std::endl;
    return Standard_False;
  }

  LDOMParser aParser;
  if (aParser.parse(*aFileStream, Standard_True, Standard_False)) {
    TCollection_AsciiString aData;
    std::cerr << "failed to parse " << filename << std::endl;
    std::cerr << aParser.GetError(aData) << ": " << aData << std::endl;
    return Standard_False;
  }

  // Get the DOM document
  ocxDocEL = aParser.getDocument().getDocumentElement();

  // Check that we got the right root element
  std::string rtn(ocxDocEL.getTagName().GetString());
  std::size_t pos = rtn.find(':');
  if (pos == std::string::npos) {
    std::cerr << "no : found in root element name <" << rtn << ">" << std::endl;
    return Standard_False;
  }
  nsPrefix = rtn.substr(0, pos);
  std::cout << "ocx prefix '" << nsPrefix << "'" << std::endl;

  if (rtn != nsPrefix + ":ocxXML") {
    std::cerr << "expected root element <" << nsPrefix << ":ocxXML>, but got <"
              << rtn << ">" << std::endl;
    return Standard_False;
  }

  // Check the version compatibility
  std::string schemaVersion(ocxDocEL.getAttribute("schemaVersion").GetString());
  if (schemaVersion.empty()) {
    std::cerr << "no schemaVersion attribute found in root element"
              << std::endl;
    return Standard_False;
  }
  if (Version(schemaVersion) < Version("2.8.5") &&
      Version(schemaVersion) > Version("2.9.0")) {
    std::cerr << "expect schemaVersion ^2.8.5, got " << schemaVersion
              << std::endl;
    return Standard_False;
  }

  // Create context
  ctx = std::make_shared<OCXContext>(ocxDocEL, nsPrefix);

  return Standard_True;
}

Standard_Boolean OCXCAFControl_Reader::Perform(
    Standard_CString filename, Handle(TDocStd_Document) & doc,
    const Message_ProgressRange &theProgress) {
  if (ReadFile(filename) == Standard_False) {
    return Standard_False;
  }
  return Transfer(doc, theProgress);
}

Standard_Boolean OCXCAFControl_Reader::Transfer(
    Handle(TDocStd_Document) & doc, const Message_ProgressRange &theProgress) {
  if (ocxDocEL == nullptr) {
    std::cerr << "run ReadFile before Transfer" << std::endl;
    return Standard_False;
  }

  //
  // OCAF
  //
  Handle(TDocStd_Application) app = new TDocStd_Application;
  app->NewDocument("XmlOcaf", doc);
  if (doc.IsNull()) {
    std::cerr << "Can not create OCAF document" << std::endl;
    return Standard_False;
  }
  ctx->OCAFDoc(doc);

  TDF_Label vesselL = doc->Main();
  // TODO: use ship name from OCX
  TDataStd_Name::Set(vesselL, "Vessel");

  //
  // XML
  //
  std::string fullTagName = ocxDocEL.getTagName().GetString();
  std::size_t index = fullTagName.find(':');
  if (index == std::string::npos) {
    std::cerr << "root element tag does not contain a colon (:) '"
              << fullTagName << "'" << std::endl;
    return Standard_False;
  }
  nsPrefix = fullTagName.substr(0, index);

  if (std::string expectedTagName = nsPrefix + ":ocxXML";
      expectedTagName != fullTagName) {
    std::cerr << "expect document to start with " << expectedTagName
              << ", but found " << fullTagName << "'" << std::endl;
    return Standard_False;
  }

  ctx->PrepareUnits();

  LDOM_Element vesselN = OCXHelper::GetFirstChild(ocxDocEL, "Vessel");
  if (vesselN.isNull()) {
    std::cerr << "could not find ocxXML/Vessel element" << std::endl;
    return Standard_False;
  }

  TopoDS_Compound rootS;
  BRep_Builder rootBuilder;
  rootBuilder.MakeCompound(rootS);
  TDF_Label rootL = ctx->OCAFShapeTool()->AddShape(rootS, true);
  TDataStd_Name::Set(rootL, "Vessel");

  // Read coordinate system
  OCXCoordinateSystemReader cosysReader(ctx);
  TopoDS_Shape cosysS = cosysReader.ReadCoordinateSystem(vesselN);
  TDF_Label cosysL = ctx->OCAFShapeTool()->AddShape(cosysS, true);
  TDataStd_Name::Set(cosysL, "Coordinate System");

  // Read reference surfaces
  OCXReferenceSurfacesReader refSrfReader(ctx);
  TopoDS_Shape referenceSurfaces = refSrfReader.ReadReferenceSurfaces(vesselN);
  TDF_Label refSrfL = ctx->OCAFShapeTool()->AddShape(referenceSurfaces, true);
  TDataStd_Name::Set(refSrfL, "Reference Surfaces");

  // Read panels
  OCXPanelReader panelReader(ctx);
  TopoDS_Shape panels = panelReader.ReadPanels(vesselN);
  TDF_Label panelsL = ctx->OCAFShapeTool()->AddShape(panels, true);
  TDataStd_Name::Set(panelsL, "Panels");

  std::string fileName = "vessel.stp";

  STEPCAFControl_Writer writer;
  try {
    if (!writer.Transfer(doc, STEPControl_AsIs)) {
      std::cerr << "failed to transfer root shape to STEP" << std::endl;
      return false;
    }
    const IFSelect_ReturnStatus ret = writer.Write(fileName.c_str());
    if (ret == IFSelect_RetDone) {
      std::cerr << "failed to write to STEP, got " << ret << std::endl;
      return false;
    }
  } catch (Standard_Failure const &exp) {
    std::cerr << "an error occurred transferring geometry:" << exp << std::endl;
  }

  return Standard_True;
}
