// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include <BinXCAFDrivers.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XmlDrivers.hxx>
#include <memory>

#include "ocx/ocx-reader.h"

int main() {
  // Initialize the application
  Handle(TDocStd_Application) app = new TDocStd_Application;

  // Initialize the document
  Handle(TDocStd_Document) doc;
  // app->NewDocument("XmlOcaf", doc);
  app->NewDocument("BinXCAF", doc);
  if (doc.IsNull()) {
    std::cerr << "Can not create OCAF document" << std::endl;
    return 1;
  }

  // Read and parse the OCX file
  auto reader = std::make_unique<ocx::OCXReader>();
  char const fileName[] =
      "data/Aveva-OHCM-MidShip_V285.3docx";  // NAPA-D-BULKER-MID_V286.3docx,
                                             // Aveva-OHCM-MidShip_V285.3docx,
                                             // ERHULLV2.3docx
  std::cout << "Read from " << fileName << std::endl;
  if (!reader->Perform(fileName, doc)) {
    std::cerr << "Can't read OCX document" << std::endl;
    app->Close(doc);
    return 1;
  }

  // Save the document to xbf
  BinXCAFDrivers::DefineFormat(app);
  app->SaveAs(doc, "doc.xbf");
  if (app->SaveAs(doc, "doc.xbf") != PCDM_SS_OK) {
    app->Close(doc);

    std::cout << "Cannot write OCAF document to xbf." << std::endl;
    return 1;
  }

  // Save the document to xml
  // XmlDrivers::DefineFormat(app);
  // if (app->SaveAs(doc, "doc.xml") != PCDM_SS_OK) {
  //   app->Close(doc);
  //
  //   std::cout << "Cannot write OCAF document to xml." << std::endl;
  //   return 1;
  // }

  app->Close(doc);

  return 0;
}
