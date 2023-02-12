/***************************************************************************
 *   Created on: 15 Dec 2022                                               *
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

#include "ocxreader/internal/ocxreader-log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog_setup/conf.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ocxreader {

void Log::Initialize(std::string_view logConfigFile) {
  try {
    // spdlog_setup::setup_error thrown if file not found
    spdlog_setup::from_file(logConfigFile.data());
    return;
  } catch (spdlog_setup::setup_error const &err) {
    std::cerr << "Error: Could not initialize logging from file: "
              << logConfigFile.data() << " - " << err.what() << std::endl;
  } catch (std::exception const &err) {
    std::cerr << "Unknown error during logging initialization: " << err.what()
              << std::endl;
  }

  // Fallback to default logging configuration
  // No need to initialize libraries logging systems as they provide their own
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("%^[%l] %n: %v%$");

  std::vector<spdlog::sink_ptr> sinks{consoleSink};
  auto logger = std::make_shared<spdlog::logger>(OCXREADER_DEFAULT_LOGGER_NAME,
                                                 begin(sinks), end(sinks));
  logger->set_level(spdlog::level::warn);
  logger->flush_on(spdlog::level::warn);
  spdlog::register_logger(logger);
}

void Log::Shutdown() { spdlog::shutdown(); }

}  // namespace ocxreader
