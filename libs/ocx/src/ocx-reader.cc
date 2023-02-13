/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
 ***************************************************************************
 *   Copyright (c) 2022, Carsten Zerbst (carsten.zerbst@groy-groy.de)      *
 *   Copyright (c) 2022, Paul Buechner                                     *
 *                                                                         *
 *   This file is part of the OCXReader library.                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   version 2.1 as published by the Free Software Foundation.             *
 *                                                                         *
 ***************************************************************************/

#include "ocx/ocx-reader.h"

#include <LDOMParser.hxx>
#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <OSD_FileSystem.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TDataStd_Name.hxx>
#include <memory>
#include <string>

#include "ocx/internal/ocx-coordinate-system.h"
#include "ocx/internal/ocx-log.h"
#include "ocx/internal/ocx-utils.h"
#include "ocx/internal/ocx-vessel.h"
#include "ocx/ocx-helper.h"

namespace ocx {

Standard_Boolean OCXReader::Perform(Standard_CString filename,
                                    Handle(TDocStd_Document) doc,
                                    std::shared_ptr<OCXContext> &ctx,
                                    const Message_ProgressRange &theProgress) {
  Log::Initialize();

  if (ReadFile(filename, ctx) == Standard_False) {
    Log::Shutdown();
    return Standard_False;
  }

  if (Parse(doc, theProgress) == Standard_False) {
    Log::Shutdown();
    return Standard_False;
  }

  Log::Shutdown();
  return Standard_True;
}

Standard_Boolean OCXReader::ReadFile(Standard_CString filename,
                                     std::shared_ptr<OCXContext> &ctx) {
  // Load the OCX Document as DOM
  const Handle(OSD_FileSystem) &aFileSystem =
      OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFileStream =
      aFileSystem->OpenIStream(filename, std::ios::in);

  if (aFileStream == nullptr || !aFileStream->good()) {
    OCX_ERROR("Could not open file {} for reading", filename)
    return Standard_False;
  }

  LDOMParser aParser;
  if (aParser.parse(*aFileStream, Standard_True, Standard_False)) {
    TCollection_AsciiString aData;
    aParser.GetError(aData);
    OCX_ERROR("Failed to parse file {}\n{}", filename, aData.ToCString())
    return Standard_False;
  }

  // Get the DOM document and assign it to OCXReader root element
  LDOM_Element documentRoot = aParser.getDocument().getDocumentElement();

  // Check root element
  std::string_view rtn = documentRoot.getTagName().GetString();
  std::size_t pos = rtn.find(':');
  if (pos == std::string::npos) {
    OCX_ERROR("Root element <{}> contains no :", rtn)
    return Standard_False;
  }
  auto nsPrefix = std::string(rtn.substr(0, pos));
  if (rtn != nsPrefix + ":ocxXML") {
    OCX_ERROR("Expected root element <{}:ocxXML>, but got <{}>", nsPrefix, rtn)
    return Standard_False;
  }

  // Check version compatibility
  char const *schemaVersion(
      documentRoot.getAttribute("schemaVersion").GetString());
  if (schemaVersion == nullptr) {
    OCX_ERROR("No schemaVersion attribute found in root element")
    return Standard_False;
  }
  if (utils::Version(schemaVersion) < utils::Version("2.8.5") &&
      utils::Version(schemaVersion) > utils::Version("2.9.0")) {
    OCX_ERROR("Unsupported schemaVersion, expected ^2.8.5, but got {}",
              schemaVersion)
    return Standard_False;
  }

  // Initialize context
  try {
    OCXContext::Initialize(documentRoot, nsPrefix);
  } catch (OCXInitializationFailedException const &e) {
    OCX_ERROR(e.what())
    return Standard_False;
  }
  OCX_INFO("Initialized context successfully")

  // Assign context for usage outside ocx-reader
  ctx = OCXContext::GetInstance();

  return Standard_True;
}

//-----------------------------------------------------------------------------

Standard_Boolean OCXReader::Parse(Handle(TDocStd_Document) & doc,
                                  const Message_ProgressRange &theProgress) {
  // Add the OCX document to the context
  OCXContext::GetInstance()->OCAFDoc(doc);

  // Parse and prepare units set in the OCX document
  OCXContext::GetInstance()->PrepareUnits();

  // Set the OCAF root label (0:1)
  LDOM_Element header = ocx::helper::GetFirstChild(
      OCXContext::GetInstance()->OCXRoot(), "Header");
  TDataStd_Name::Set(doc->Main(), header.getAttribute("name").GetString());

  // TODO: Read ClassCatalogue

  // Read Vessel elements TODO: Should return Standard_Boolean
  ocx::reader::vessel::ReadVessel();

  return Standard_True;
}

}  // namespace ocx
