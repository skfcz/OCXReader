/***************************************************************************
 *   Created on: 19 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_LOG_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_LOG_H_

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace ocx {

/**
 * OCX Logger class
 */
class Log {
 public:
  /**
   * Initialize the ocx logging system
   */
  static void Initialize();

  /**
   * Shutdown the ocx logging system
   */
  static void Shutdown();

 private:
  /**
   * @brief Flag to indicate if the logging system was initialized through a
   * config file. This is used to determine if the logging system should be
   * shutdown when the ocx library is unloaded or not.
   */
  static bool m_initializedThroughConfigFile;
};

}  // namespace ocx

constexpr char OCX_DEFAULT_LOGGER_NAME[] = "OCX";

#define OCX_TRACE(...)                                        \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__); \
  }
#define OCX_DEBUG(...)                                        \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__); \
  }
#define OCX_INFO(...)                                        \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__); \
  }
#define OCX_WARN(...)                                        \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__); \
  }
#define OCX_ERROR(...)                                        \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); \
  }
#define OCX_FATAL(...)                                           \
  if (spdlog::get(OCX_DEFAULT_LOGGER_NAME) != nullptr) {         \
    spdlog::get(OCX_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); \
  }

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_LOG_H_
