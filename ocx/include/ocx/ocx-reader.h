/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_OCX_READER_H_
#define OCX_INCLUDE_OCX_OCX_READER_H_

#include <LDOM_Element.hxx>
#include <memory>
#include <string>

#include "ocx/ocx-context.h"

namespace ocx {

/**
 * Provides a tool to read OCX file and put it into
 * OCAF document. Besides transfer of shapes (including
 * assemblies) supports also part names and properties.
 */
class OCXReader {
 public:
  OCXReader() = default;
  ~OCXReader() = default;

  /*
   * Combination of ReadFile and Transfer
   */
  static Standard_EXPORT Standard_Boolean
  Perform(Standard_CString filename, Handle(TDocStd_Document) & doc,
          std::shared_ptr<OCXContext> &ctx,
          Message_ProgressRange const &theProgress = Message_ProgressRange());

 private:
  /**
   * Translate OCX file given by filename into the document
   * Return True if succeeded, and False in case of fail
   * @param filename the file to read
   * @param ctx the context to use
   * @return true if result is usable for Transfer
   */
  static Standard_EXPORT Standard_Boolean
  ReadFile(Standard_CString filename, std::shared_ptr<OCXContext> &ctx);

  /**
   * Parsed the document model into OCAF
   *
   * @param doc the target model
   * @param theProgress progress
   * @return true if result could be used.
   */
  static Standard_EXPORT Standard_Boolean
  Parse(Handle(TDocStd_Document) & doc,
        Message_ProgressRange const &theProgress = Message_ProgressRange());
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_READER_H_
