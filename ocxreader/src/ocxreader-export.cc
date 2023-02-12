/***************************************************************************
 *   Created on: 13 Dec 2022                                               *
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

#include "ocxreader/internal/ocxreader-export.h"

#include <BinXCAFDrivers.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XmlDrivers.hxx>

#include "shipxml/shipxml-driver.h"

namespace ocxreader::file_export {

int HandleExport(Handle(TDocStd_Document) const& doc,
                 Handle(TDocStd_Application) const& app,
                 std::string_view outputFilePath,
                 std::vector<std::string> const& exportFormats) {
  for (auto& format : exportFormats) {
    if (format == "STEP") {
      // Write to STEP
      STEPCAFControl_Writer stepWriter;
      try {
        if (!stepWriter.Transfer(doc, STEPControl_AsIs)) {
          std::cerr << "Failed to transfer document to STEP model" << std::endl;
          return 66;
        }
        const IFSelect_ReturnStatus ret =
            stepWriter.Write((std::string(outputFilePath) + ".stp").c_str());
        if (ret != IFSelect_RetDone) {
          std::cerr << "Failed to write STEP file, exited with status: " << ret
                    << std::endl;
          return 66;
        }
      } catch (Standard_Failure const& exp) {
        std::cerr << "Failed to write STEP file, exception: "
                  << exp.GetMessageString() << std::endl;
        return 66;
      }
    }

    else if (format == "XCAF-XML") {
      XmlDrivers::DefineFormat(app);
      if (app->SaveAs(doc, (std::string(outputFilePath) + ".xml").c_str()) !=
          PCDM_SS_OK) {
        std::cerr << "Cannot write OCAF document to xml." << std::endl;
        return 66;
      }
    }

    else if (format == "XCAF-XBF") {
      BinXCAFDrivers::DefineFormat(app);
      if (app->SaveAs(doc, (std::string(outputFilePath) + ".xbf").c_str()) !=
          PCDM_SS_OK) {
        std::cerr << "Cannot write OCAF document to xbf." << std::endl;
        return 66;
      }
    }

    else if (format == "SHIPXML") {
      shipxml::ShipXMLDriver xmlDriver;
      // try {
      if (!(xmlDriver.Transfer())) {
        std::cerr << "Failed to transfer document to ShipXML model"
                  << std::endl;
        return 66;
      }
      if (bool ret = xmlDriver.Write(std::string(outputFilePath) + ".shipxml");
          ret != IFSelect_RetDone) {
        std::cerr << "Failed to write ShipXML file, exited with status" << ret
                  << std::endl;
        return 66;
      }
      // } catch (Standard_Failure const& exp) {
      //   std::cerr << "Failed to write ShipXML file, exception: "
      //             << exp.GetMessageString() << std::endl;
      //   return 66;
      // }
    }
  }
  return 0;
}

}  // namespace ocxreader::file_export
