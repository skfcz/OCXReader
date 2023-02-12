/***************************************************************************
 *   Created on: 12 Dec 2022                                               *
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

#include "ocxreader/internal/ocxreader-cli.h"

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <filesystem>
#include <iostream>
#include <string>

namespace ocxreader::cli {

template <class charT>
void parseChildren(std::string const &prefix, boost::property_tree::ptree &tree,
                   boost::program_options::parsed_options &options) {
  if (tree.empty()) {
    // remove first dot
    std::basic_string<charT> name = prefix.substr(1);
    // remove last dot if present
    if (name.back() == '.') {
      name.pop_back();
    }
    std::basic_string<charT> value = tree.data();

    boost::program_options::basic_option<charT> opt;
    opt.string_key = name;
    opt.value.push_back(value);
    opt.unregistered =
        (options.description->find_nothrow(name, false) == nullptr);
    opt.position_key = -1;
    // append value to existing option-key if it exists
    for (auto &o : options.options) {
      if (o.string_key == name) {
        o.value.push_back(value);
        return;
      }
    }
    options.options.push_back(opt);
  } else {
    for (auto it = tree.begin(); it != tree.end(); ++it) {
      parseChildren<charT>(prefix + "." + it->first, it->second, options);
    }
  }
}

template <class charT>
void parseJsonOptions(std::basic_istream<charT> &is,
                      boost::program_options::parsed_options &options) {
  boost::property_tree::basic_ptree<std::basic_string<charT>,
                                    std::basic_string<charT>>
      pt;
  boost::property_tree::read_json(is, pt);

  parseChildren<charT>(std::basic_string<charT>(), pt, options);
}

template <class charT>
boost::program_options::basic_parsed_options<charT> parse_json_config_file(
    std::basic_istream<charT> &is,
    boost::program_options::options_description const &desc,
    bool allow_unregistered) {
  // do any option checks here

  boost::program_options::parsed_options result(&desc);
  parseJsonOptions(is, result);

  return boost::program_options::basic_parsed_options<charT>(result);
}

bool get_valid_file_path(std::string const &filepath,
                         std::filesystem::path &path) {
  path = std::filesystem::path(filepath);
  // Use system dependent preferred path separators
  path.make_preferred();
  if (!std::filesystem::exists(path)) {
    std::cerr << "No config file found at: " << path << std::endl;
    return false;
  }
  return true;
}

}  // namespace ocxreader::cli
