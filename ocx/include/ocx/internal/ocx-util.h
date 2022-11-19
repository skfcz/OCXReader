//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_

#include <cstdio>
#include <string>

namespace ocx {

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
 * Convert a string to a boolean
 *
 * @param s string to convert (true, false, 0, 1)
 * @param throw_on_error throw an exception if conversion fails
 * @return boolean value
 */
bool stob(std::string s, bool throw_on_error = true);

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
