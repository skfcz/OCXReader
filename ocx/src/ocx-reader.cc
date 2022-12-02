// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/ocx-reader.h"

#include <LDOMParser.hxx>
#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <OSD_FileSystem.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <TDataStd_Name.hxx>
#include <memory>
#include <string>
#include <map>

#include "ocx/internal/ocx-coordinate-system-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-log.h"
#include "ocx/internal/ocx-panel-reader.h"
#include "ocx/internal/ocx-reference-surfaces-reader.h"
#include "ocx/internal/ocx-util.h"

namespace ocx {

Standard_Boolean OCXReader::ReadFile(Standard_CString filename) {
  // Load the OCX Document as DOM
  const Handle(OSD_FileSystem) &aFileSystem =
      OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFileStream =
      aFileSystem->OpenIStream(filename, std::ios::in);

  if (aFileStream == nullptr || !aFileStream->good()) {
    OCX_ERROR("Could not open file {} for reading", filename);
    return Standard_False;
  }

  LDOMParser aParser;
  if (aParser.parse(*aFileStream, Standard_True, Standard_False)) {
    TCollection_AsciiString aData;
    aParser.GetError(aData);
    OCX_ERROR("Failed to parse file {}\n{}", filename, aData.ToCString());
    return Standard_False;
  }

  // Get the DOM document
  ocxDocEL = aParser.getDocument().getDocumentElement();

  // Check root element
  std::string_view rtn = ocxDocEL.getTagName().GetString();
  std::size_t pos = rtn.find(':');
  if (pos == std::string::npos) {
    OCX_ERROR("Root element <{}> contains no :", rtn);
    return Standard_False;
  }
  auto nsPrefix = std::string(rtn.substr(0, pos));
  if (rtn != nsPrefix + ":ocxXML") {
    OCX_ERROR("Expected root element <{}:ocxXML>, but got <{}>", nsPrefix, rtn);
    return Standard_False;
  }

  // Check version compatibility
  char const *schemaVersion(ocxDocEL.getAttribute("schemaVersion").GetString());
  if (schemaVersion == nullptr) {
    OCX_ERROR("No schemaVersion attribute found in root element");
    return Standard_False;
  }
  if (Version(schemaVersion) < Version("2.8.5") &&
      Version(schemaVersion) > Version("2.9.0")) {
    OCX_ERROR("Unsupported schemaVersion, expected ^2.8.5, but got {}",
              schemaVersion);
    return Standard_False;
  }

  // Create context
  ctx = std::make_shared<OCXContext>(ocxDocEL, nsPrefix);
  OCX_INFO("Created context successfully");

  return Standard_True;
}

Standard_Boolean OCXReader::Transfer(Handle(TDocStd_Document) & doc,
                                     const Message_ProgressRange &theProgress) {
  // Add the OCX document to the context
  ctx->OCAFDoc(doc);

  // Parse and prepare units set in the OCX document
  ctx->PrepareUnits();

  // Set the OCAF root label (0:1)
  LDOM_Element header = OCXHelper::GetFirstChild(ocxDocEL, "Header");
  TDataStd_Name::Set(doc->Main(), header.getAttribute("name").GetString());

  // Start parsing the OCX document
  LDOM_Element vesselN = OCXHelper::GetFirstChild(ocxDocEL, "Vessel");
  if (vesselN.isNull()) {
    OCX_ERROR("No ocxXML/Vessel element found in provided OCX document");
    return Standard_False;
  }

  // Read coordinate system
  OCXCoordinateSystemReader coSysReader(ctx);
  coSysReader.ReadCoordinateSystem(vesselN);

  // Read reference surfaces
  OCXReferenceSurfacesReader refSrfReader(ctx);
  refSrfReader.ReadReferenceSurfaces(vesselN);

  // Read panels
  OCXPanelReader panelReader(ctx);
  panelReader.ReadPanels(vesselN);



  return Standard_True;
}

Standard_Boolean OCXReader::Perform(Standard_CString filename,
                                    Handle(TDocStd_Document) & doc,
                                    const Message_ProgressRange &theProgress) {
  Log::Initialize();

  if (ReadFile(filename) == Standard_False) {
    Log::Shutdown();

    return Standard_False;
  }

  if (Transfer(doc, theProgress) == Standard_False) {
    Log::Shutdown();

    return Standard_False;
  }

  return Standard_True;
}
LDOM_Element OCXReader::OCXRoot() { return ocxDocEL;}

}  // namespace ocx
