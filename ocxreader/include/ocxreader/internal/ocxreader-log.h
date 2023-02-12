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

#ifndef OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_LOG_H_
#define OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_LOG_H_

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>
#include <vector>

namespace ocxreader {

/**
 * OCXReader Logger class
 */
class Log {
 public:
  /**
   * Initialize the ocxreader logging system
   *
   * @param logConfigFile path to the log config file
   */
  static void Initialize(std::string_view logConfigFile = "");

  /**
   * Shutdown the ocxreader logging system
   */
  static void Shutdown();
};

}  // namespace ocxreader

constexpr char OCXREADER_DEFAULT_LOGGER_NAME[] = "OCXREADER";

#define OCXREADER_TRACE(...)                                        \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__); \
  }
#define OCXREADER_DEBUG(...)                                        \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__); \
  }
#define OCXREADER_INFO(...)                                        \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__); \
  }
#define OCXREADER_WARN(...)                                        \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {     \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__); \
  }
#define OCXREADER_ERROR(...)                                        \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {      \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); \
  }
#define OCXREADER_FATAL(...)                                           \
  if (spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME) != nullptr) {         \
    spdlog::get(OCXREADER_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); \
  }

#endif  // OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_LOG_H_
