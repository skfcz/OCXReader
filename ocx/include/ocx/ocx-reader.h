// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_OCX_READER_H_
#define OCX_INCLUDE_OCX_OCX_READER_H_

#include <LDOM_Element.hxx>
#include <memory>
#include <string>

#include "ocx/internal/ocx-context.h"

namespace ocx {

/**
 * Provides a tool to read OCX file and put it into
 * OCAF document. Besides transfer of shapes (including
 * assemblies) supports also part names and properties.
 */
class OCXReader {
 public:
  /*
   * Combination of ReadFile and Transfer
   */
  Standard_EXPORT Standard_Boolean
  Perform(Standard_CString filename, Handle(TDocStd_Document) & doc,
          Message_ProgressRange const &theProgress = Message_ProgressRange());

 private:
  /**
   * The context of the reader
   */
  std::shared_ptr<OCXContext> ctx;

  /**
   * The parsed document element
   */
  LDOM_Element ocxDocEL;

  /**
   * Translate OCX file given by filename into the document
   * Return True if succeeded, and False in case of fail
   * @param filename the file to read
   * @return true if result is usable for Transfer
   */
  Standard_EXPORT Standard_Boolean ReadFile(Standard_CString filename);

  /**
   * Transfer the parsed model into OCAFS
   * TODO: discuss if parsing should get handled inside Reader::Transfer
   *
   * @param doc the target model
   * @param theProgress progress
   * @return true if result could be used.
   */
  Standard_EXPORT Standard_Boolean
  Transfer(Handle(TDocStd_Document) & doc,
           Message_ProgressRange const &theProgress = Message_ProgressRange());
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_READER_H_
