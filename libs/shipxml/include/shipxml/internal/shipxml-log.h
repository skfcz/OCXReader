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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LOG_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LOG_H_

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace shipxml {

/**
 * Logging class for shipxml library
 */
class Log {
 public:
  static void Initialize();

  static void Shutdown();
};

}  // namespace shipxml

constexpr char SHIPXML_DEFAULT_LOGGER_NAME[] = "SHIPXML";

#ifndef SHIPXML_CONFIG_RELEASE
#define SHIPXML_TRACE(...)                                        \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__); \
  }
#define SHIPXML_DEBUG(...)                                        \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__); \
  }
#define SHIPXML_INFO(...)                                        \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__); \
  }
#define SHIPXML_WARN(...)                                        \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__); \
  }
#define SHIPXML_ERROR(...)                                    \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); \
  }
#define SHIPXML_FATAL(...)                                           \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {         \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); \
  }
#else
// Disable logging in release build
#define SHIPXML_TRACE(...) (void)0
#define SHIPXML_DEBUG(...) (void)0
#define SHIPXML_INFO(...) (void)0
#define SHIPXML_WARN(...) (void)0
#define SHIPXML_ERROR(...) (void)0
#define SHIPXML_FATAL(...) (void)0
#endif

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LOG_H_
