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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXCEPTIONS_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXCEPTIONS_H_

#include <stdexcept>

/**
 * All SHIPXML exceptions derive from this
 */
struct SHIPXMLBaseException : std::invalid_argument {
  using invalid_argument::invalid_argument;
};

/**
 * Exception thrown if initialization fails
 */
struct SHIPXMLInitializationFailedException : SHIPXMLBaseException {
  using SHIPXMLBaseException::SHIPXMLBaseException;
};

/**
 * Basic NotFound exception
 */
struct SHIPXMLNotFoundException : SHIPXMLBaseException {
  using SHIPXMLBaseException::SHIPXMLBaseException;
};

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_EXCEPTIONS_H_
