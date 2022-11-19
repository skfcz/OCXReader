// This file is part of OCXReader library
// Copyright Paul Buechner
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace ocx {

void Log::Initialize() {
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("%^[%T] %n: %v%$");

  std::vector<spdlog::sink_ptr> sinks{consoleSink};
  auto logger = std::make_shared<spdlog::logger>(OCX_DEFAULT_LOGGER_NAME,
                                                 begin(sinks), end(sinks));
  logger->set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);
  spdlog::register_logger(logger);
}

void Log::Shutdown() { spdlog::shutdown(); }

}  // namespace ocx
