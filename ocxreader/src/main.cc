// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include <ocx/internal/ocx-log.h>

#include <STEPCAFControl_Writer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XmlDrivers.hxx>
#include <boost/program_options.hpp>
#include <exception>
#include <memory>
using std::cerr;
using std::cout;
using std::endl;
using std::exception;

#include "ocx/ocx-reader.h"

namespace po = boost::program_options;

int main(int ac, char** av) {
  try {
    po::options_description desc("Supported options");
    desc.add_options()("help", "produce help message")(
        "in", po::value<std::string>(), "the OCX file to read")(
        "step", po::value<std::string>(), "the step file to write")(
        "ocaf", po::value<std::string>(), "the OCAF file to write");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << "\n";
      return 0;
    }

    std::string ocxFileName = "";
    // "data/Aveva-OHCM-MidShip_V285.3docx";  // NAPA-D-BULKER-MID_V286.3docx,
    // Aveva-OHCM-MidShip_V285.3docx,
    // ERHULLV2.3docx
    if (!vm.count("in")) {
      cerr << "no input OCX file given.\n";
      return 33;
    } else {
      ocxFileName = vm["in"].as<std::string>();
    }

    std::string stepFileName = "";
    if (vm.count("step")) {
      stepFileName = vm["step"].as<std::string>();
    }

    std::string ocafFileName = "";
    if (vm.count("ocaf")) {
      ocafFileName = vm["ocaf"].as<std::string>();
    }

    // Initialize the application
    Handle(TDocStd_Application) app = new TDocStd_Application;

    // Initialize the document
    Handle(TDocStd_Document) doc;
    app->NewDocument("XmlOcaf", doc);
    if (doc.IsNull()) {
      std::cerr << "Can not create OCAF document" << std::endl;
      return 1;
    }

    // Read and parse the OCX file
    auto reader = std::make_unique<ocx::OCXReader>();

    std::cout << "Read from " << ocxFileName << std::endl;
    if (!reader->Perform(ocxFileName.c_str(), doc)) {
      std::cerr << "Can't read OCX document" << std::endl;
      app->Close(doc);
      return 1;
    }

    // Write to STEP
    if (!stepFileName.empty()) {
      STEPCAFControl_Writer writer;
      try {
        if (!writer.Transfer(doc, STEPControl_AsIs)) {
          cerr << "Failed to transfer document to STEP model" << endl;
          return 66;
        }
        const IFSelect_ReturnStatus ret = writer.Write(stepFileName.c_str());
        if (ret != IFSelect_RetDone) {
          OCX_ERROR("Failed to write STEP file, exited with status {}", ret);
          return 66;
        }
      } catch (Standard_Failure const& exp) {
        OCX_ERROR("Failed to write STEP file, exception: {}",
                  exp.GetMessageString());
        return 66;
      }
    }

    // Save the document
    if (!ocafFileName.empty()) {
      XmlDrivers::DefineFormat(app);
      if (app->SaveAs(doc, ocafFileName.c_str()) != PCDM_SS_OK) {
        app->Close(doc);

        std::cerr << "Cannot write OCAF document." << std::endl;
        return 66;
      }
    }

    app->Close(doc);

  } catch (exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  } catch (...) {
    cerr << "Exception of unknown type!\n";
    return 1;
  }
  return 0;
}
