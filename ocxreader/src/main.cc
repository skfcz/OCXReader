/***************************************************************************
 *   Created on: 31 May 2022                                               *
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

#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <boost/program_options.hpp>
#include <filesystem>
#include <memory>

#include "ocx/ocx-reader.h"
#include "ocxreader/internal/ocxreader-cli.h"
#include "ocxreader/internal/ocxreader-export.h"
#include "ocxreader/internal/ocxreader-log.h"

int main(int argc, char** argv) {
  namespace po = boost::program_options;

  std::string config_file_path;

  po::options_description generic("Generic options");
  generic.add_options()                      //
      ("version,v", "print version string")  //
      ("help,h", "produce help message")     //
      ("config-file", po::value<std::string>(&config_file_path),
       "The path to the file containing OCX parsing options (e.g. "
       "path/to/config_file.json)");

  po::options_description opts("OCXReader configuration options");
  opts.add_options()                                                      //
      ("input-file,i", po::value<std::string>(), "The OCX file to read")  //
      ("export-format", po::value<std::vector<std::string>>()->multitoken(),
       "The export format(s) to use. This can be one or more of the following: "
       "STEP, SHIPXML, XCAF-XML, XCAF-XBF")  //
      ("save-to,s", po::value<std::string>(),
       "The output-file path. Defines were to write the exported file(s) to. "
       "If not defined files get saved relative to the program working "
       "directory.")  //
      ("output-file,o", po::value<std::string>(),
       "The output file name. This is used as the filename to the defined "
       "export formats. If not defined input-file is used.")  //
      ("log-config-file,l", po::value<std::string>(),
       "The path to the file containing logging configuration options options "
       "(e.g. path/to/log_conf.toml)");

  po::options_description allopts("Allowed options");
  allopts.add(generic).add(opts);

  po::variables_map vm;

  try {
    // Parse all options to handle generic options and make config-file
    // available if defined
    po::store(po::parse_command_line(argc, argv, allopts), vm);
    po::notify(vm);

    if (vm.count("help") || vm.count("h")) {
      std::cout << allopts << std::endl;
      return 0;
    }

    if (vm.count("version") || vm.count("v")) {
      std::cout << "Not yet implemented." << std::endl;
      return 0;
    }

    // Parse options from reader config file if defined
    if (!config_file_path.empty()) {
      std::filesystem::path readerConfigFile;
      if (!ocxreader::cli::get_valid_file_path(config_file_path,
                                               readerConfigFile)) {
        std::cerr << "No config file found at: " << readerConfigFile
                  << std::endl;
        return 33;
      }
      std::ifstream configFileStream(readerConfigFile.generic_string());
      po::store(
          ocxreader::cli::parse_json_config_file(configFileStream, opts, true),
          vm);
    }
    // Parse all options again (overwrites config-file options)
    po::store(po::parse_command_line(argc, argv, allopts), vm);
  } catch (po::unknown_option& e) {
    std::cout << "Error parsing command line options: " << e.what()
              << std::endl;
    std::cout << allopts << std::endl;
    return 33;
  } catch (po::invalid_command_line_syntax& e) {
    std::cout << "Error parsing command line options: " << e.what()
              << std::endl;
    return 33;
  } catch (...) {
    std::cout << "Unknown error parsing command line options" << std::endl;
    std::cout << allopts << std::endl;
    return 33;
  }

  po::notify(vm);

  std::string ocxFileInput;
  if (vm.count("input-file")) {
    ocxFileInput = vm["input-file"].as<std::string>();
  } else {
    std::cerr << "No input OCX file given." << std::endl;
    return 33;
  }

  // Validate export formats, also cache XCAF export type if present (XML, XBF)
  bool exportXCAFXBF = false;
  bool exportXCAFXML = false;
  std::vector<std::string> exportFormats;
  if (vm.count("export-format")) {
    exportFormats = vm["export-format"].as<std::vector<std::string>>();
    for (auto const& format : exportFormats) {
      if (format == "STEP") {
        // ...
      } else if (format == "SHIPXML") {
        // ...
      } else if (format == "XCAF-XML") {
        exportXCAFXML = true;
      } else if (format == "XCAF-XBF") {
        exportXCAFXBF = true;
      } else {
        std::cerr << "Invalid export format: " << format << std::endl;
        return 33;
      }
    }
    if (exportXCAFXBF && exportXCAFXML) {
      std::cerr
          << "XCAF-XBF and XCAF-XML export formats are mutually exclusive. "
             "Please choose only one."
          << std::endl;
      return 33;
    }
  } else {
    std::cerr << "No export format given. Please specify at least one export "
                 "format target."
              << std::endl;
    return 33;
  }

  std::string saveTo;
  if (vm.count("save-to")) {
    std::filesystem::path saveToPath(vm["save-to"].as<std::string>());
    // Use system dependent preferred path separators
    saveToPath.make_preferred();
    // Check if saveTo is a valid path else try to create it
    if (!std::filesystem::exists(saveToPath)) {
      try {
        std::filesystem::create_directories(saveToPath);
      } catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating output directory: " << e.what()
                  << std::endl;
        return 33;
      }
    }
    // Append system dependent preferred path separator if not present
    if (saveToPath.string().back() !=
        std::filesystem::path::preferred_separator) {
      saveToPath += std::filesystem::path::preferred_separator;
    }
    saveTo = saveToPath.string();
  }

  std::string outputFileName;
  if (vm.count("output-file")) {
    outputFileName = vm["output-file"].as<std::string>();
    // Check if user accidentally added file extension
    if (std::size_t idx = outputFileName.find_last_of('.');
        idx != std::string::npos) {
      outputFileName = ocxFileInput.substr(0, idx);
    }
  } else {
    // Get filename without extension from given input-file/path
    std::string outputFile;
    if (std::size_t idx = ocxFileInput.find_last_of("/\\");
        idx != std::string::npos) {
      outputFile = ocxFileInput.substr(idx + 1);
    } else {
      outputFile = ocxFileInput;
    }
    outputFileName = outputFile.substr(0, outputFile.find_last_of('.'));
  }
  // Assemble output file path location
  std::string outputFilePath = saveTo + outputFileName;

  // Initialize logging
  std::filesystem::path logConfigFile;
  if (vm.count("log-config-file")) {
    if (!ocxreader::cli::get_valid_file_path(
            vm["log-config-file"].as<std::string>(), logConfigFile)) {
      std::cerr << "No config file found at: " << logConfigFile << std::endl;
      return 33;
    }
    ocxreader::Log::Initialize(logConfigFile.generic_string());
  } else {
    ocxreader::Log::Initialize();
  }

  // Initialize the application
  Handle(TDocStd_Application) app = new TDocStd_Application;

  // Initialize the document
  Handle(TDocStd_Document) doc;
  if (exportXCAFXML) {
    app->NewDocument("XmlOcaf", doc);
  } else {
    app->NewDocument("BinXCAF", doc);  // default to XBF
  }

  if (doc.IsNull()) {
    std::cerr << "Failed to create document" << std::endl;
    ocxreader::Log::Shutdown();
    return 33;
  }

  // Read and parse the OCX file
  std::shared_ptr<ocx::OCXContext> ctx;
  std::cout << "Read from " << ocxFileInput << std::endl;
  if (!ocx::OCXReader::Perform(ocxFileInput.c_str(), doc, ctx)) {
    std::cerr << "Failed to read OCX document" << std::endl;
    app->Close(doc);
    ocxreader::Log::Shutdown();
    return 33;
  }

  // Handle the export
  if (int ret = ocxreader::file_export::HandleExport(doc, app, outputFilePath,
                                                     exportFormats);
      ret == 66) {
    std::cerr << "Failed to export" << std::endl;
    app->Close(doc);
    ocxreader::Log::Shutdown();
    return ret;
  }

  app->Close(doc);

  ocxreader::Log::Shutdown();

  return 0;
}
