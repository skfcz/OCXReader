/***************************************************************************
 *   Created on: 04 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_

#include <BOPAlgo_Builder.hxx>
#include <LDOM_Element.hxx>
#include <cstdio>
#include <string>

//-----------------------------------------------------------------------------

/**
 * Make a shared pointer to a class with a protected or private constructor
 * @see https://stackoverflow.com/a/27832765/13279727
 *
 * @tparam T Type of the class (e.g. std::shared_ptr<ocx::Context>)
 */
#define SHARED_PTR_CREATE(T)                                              \
  template <typename... Arg>                                              \
  static std::shared_ptr<T> create(Arg &&...arg) {                        \
    struct EnableMakeShared : public T {                                  \
      explicit EnableMakeShared(Arg &&...arg)                             \
          : T(std::forward<Arg>(arg)...) {}                               \
    };                                                                    \
    return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...); \
  }

//-----------------------------------------------------------------------------

namespace ocx::utils {

struct Version {
  int major{}, minor{}, revision{}, build{};

  /**
   * Construct a new Version object of the form: major.minor.revision.build
   *
   * @param versionStr The version string to parse, e.g. "1.0.2.1"
   */
  explicit Version(std::string const &versionStr) {
    sscanf(versionStr.c_str(), "%d.%d.%d.%d", &major, &minor, &revision,
           &build);
  }

  /**
   * Construct a new Version object of the form: major.minor.revision.build
   *
   * @param versionStr the pointer to the version string to parse, e.g "1.0.2.1"
   */
  explicit Version(char const *versionStr) {
    sscanf(versionStr, "%d.%d.%d.%d", &major, &minor, &revision, &build);
  }

  bool operator<(const Version &otherVersion) const {
    if (major < otherVersion.major) return true;
    if (minor < otherVersion.minor) return true;
    if (revision < otherVersion.revision) return true;
    if (build < otherVersion.build) return true;
    return false;
  }

  bool operator>(const Version &otherVersion) const {
    if (major > otherVersion.major) return true;
    if (minor > otherVersion.minor) return true;
    if (revision > otherVersion.revision) return true;
    if (build > otherVersion.build) return true;
    return false;
  }

  bool operator==(const Version &otherVersion) const {
    if (major == otherVersion.major && minor == otherVersion.minor &&
        revision == otherVersion.revision && build == otherVersion.build)
      return true;
    return false;
  }
};

/**
 * Split a string by given delimiter and return a Container of type T
 * @see https://stackoverflow.com/a/1493195/13279727
 *
 * @tparam ContainerT Container type
 * @param str String to split
 * @param tokens Container to store the result
 * @param delimiters Delimiter used to split the string (default: " ")
 * @param trimEmpty Trim empty tokens (default: true)
 */
template <class ContainerT>
static void Tokenize(std::string_view str, ContainerT &tokens,
                     std::string const &delimiters = " ",
                     bool trimEmpty = true);

/**
 * Convert a string to a boolean
 *
 * @param s string to convert (true, false, 0, 1)
 * @param throw_on_error throw an exception if conversion fails
 * @return boolean value
 */
bool stob(std::string s, bool throw_on_error = true);

}  // namespace ocx::utils

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
