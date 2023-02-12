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

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace shipxml {

/**
 * SHIPXML Logger class
 */
class Log {
 public:
  /**
   * Initialize the shipxml logging system
   */
  static void Initialize();

  /**
   * Shutdown the shipxml logging system
   */
  static void Shutdown();

 private:
  /**
   * @brief Flag to indicate if the logging system was initialized through a
   * config file. This is used to determine if the logging system should be
   * shutdown when the shipxml library is unloaded or not.
   */
  static bool m_initializedThroughConfigFile;
};

}  // namespace shipxml

constexpr char SHIPXML_DEFAULT_LOGGER_NAME[] = "SHIPXML";

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
#define SHIPXML_ERROR(...)                                        \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); \
  }
#define SHIPXML_FATAL(...)                                           \
  if (spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME) != nullptr) {         \
    spdlog::get(SHIPXML_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); \
  }

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_LOG_H_
