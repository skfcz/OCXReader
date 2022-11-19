//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include <string>

namespace ocx {

bool stob(std::string s, bool throw_on_error) {
  auto result = false;  // failure to assert is false

  std::istringstream is(s);
  // first try simple integer conversion
  is >> result;

  if (is.fail()) {
    // simple integer failed; try boolean
    is.clear();
    is >> std::boolalpha >> result;
  }

  if (is.fail() && throw_on_error) {
    throw std::invalid_argument(s.append(" is not convertable to bool"));
  }

  return result;
}

}  // namespace ocx
