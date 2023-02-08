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

#ifndef OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_CLI_H_
#define OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_CLI_H_

#include <filesystem>
#include <string>

#include "boost/program_options.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ptree.hpp"

namespace ocxreader::cli {

template <class charT>
void parseChildren(std::string const& prefix, boost::property_tree::ptree& tree,
                   boost::program_options::parsed_options& options);
template <class charT>
void parseJsonOptions(std::basic_istream<charT>& is,
                      boost::program_options::parsed_options& options);

/**
 * Parses a JSON file and returns a parsed_options object.
 * @see https://stackoverflow.com/a/45198471/13279727
 *
 * @tparam charT the character type of the input stream
 * @param is the input stream
 * @param desc the program options description
 * @param allow_unregistered if true, unknown options are allowed
 * @return the parsed_options object
 */
template <class charT>
boost::program_options::basic_parsed_options<charT> parse_json_config_file(
    std::basic_istream<charT>& is,
    const boost::program_options::options_description& desc,
    bool allow_unregistered = false);

/**
 * @brief Checks whether a given file path is valid and writes the result to the
 * given path object.
 *
 * @param filepath the file path to check
 * @return true if the file path is valid, false otherwise
 */
bool get_valid_file_path(std::string const& filepath,
                         std::filesystem::path& path);

}  // namespace ocxreader::cli

#endif  // OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_CLI_H_
