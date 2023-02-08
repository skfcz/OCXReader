/***************************************************************************
 *   Created on: 13 Dec 2022                                               *
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

#ifndef OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_EXPORT_H_
#define OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_EXPORT_H_

#include <memory>
#include <string>
#include <vector>

#include "TDocStd_Application.hxx"
#include "TDocStd_Document.hxx"
#include "ocx/ocx-context.h"

namespace ocxreader::file_export {

int HandleExport(Handle(TDocStd_Document) const& doc,
                 Handle(TDocStd_Application) const& app,
                 std::string_view outputFilePath,
                 std::vector<std::string> const& exportFormats);

}  // namespace ocxreader::file_export

#endif  // OCXREADER_INCLUDE_OCXREADER_INTERNAL_OCXREADER_EXPORT_H_
