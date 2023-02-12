/***************************************************************************
 *   Created on: 02 Dec 2022                                               *
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

#include "shipxml/internal/shipxml-log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace shipxml {

bool Log::m_initializedThroughConfigFile = false;

void Log::Initialize() {
  // Check if logging already initialized
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {
    m_initializedThroughConfigFile = true;
    return;
  }

  // Fallback to default logging configuration
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("%^[%l] %n: %v%$");

  std::vector<spdlog::sink_ptr> sinks{consoleSink};
  auto logger = std::make_shared<spdlog::logger>(SHIPXML_DEFAULT_LOGGER_NAME,
                                                 begin(sinks), end(sinks));
  logger->set_level(spdlog::level::warn);
  logger->flush_on(spdlog::level::warn);
  spdlog::register_logger(logger);
}

void Log::Shutdown() {
  if (m_initializedThroughConfigFile) return;
  spdlog::shutdown();
}

}  // namespace shipxml
