//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADER_DISTRIBUTION_OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
#define OCXREADER_DISTRIBUTION_OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_

#include <stdio.h>

#include <string>

struct Version {
  int major{}, minor{}, revision{}, build{};

  explicit Version(std::string const &versionStr) {
    sscanf(versionStr.c_str(), "%d.%d.%d.%d", &major, &minor, &revision,
           &build);
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
#endif  // OCXREADER_DISTRIBUTION_OCX_INCLUDE_OCX_INTERNAL_OCX_UTIL_H_
