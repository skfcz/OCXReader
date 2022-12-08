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

#include <BOPAlgo_PaveFiller.hxx>
#include <string>

namespace ocx::utils {

template <class ContainerT>
void Tokenize(std::string_view str, ContainerT &tokens,
              std::string const &delimiters, bool trimEmpty) {
  std::string::size_type pos;
  std::string::size_type lastPos = 0;
  std::string::size_type length = str.length();

  using value_type = typename ContainerT::value_type;
  using size_type = typename ContainerT::size_type;

  while (lastPos < length + 1) {
    pos = str.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos) {
      pos = length;
    }

    if (pos != lastPos || !trimEmpty)
      tokens.push_back(
          value_type(str.data() + lastPos, (size_type)pos - lastPos));
    lastPos = pos + 1;
  }
}

//-----------------------------------------------------------------------------

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

}  // namespace ocx::utils
