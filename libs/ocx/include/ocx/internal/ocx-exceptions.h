/***************************************************************************
 *   Created on: 02 Dec 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_EXCEPTIONS_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_EXCEPTIONS_H_

#include <stdexcept>

/**
 * All OCX exceptions derive from this
 */
struct OCXBaseException : std::invalid_argument {
  using invalid_argument::invalid_argument;
};

/**
 * Exception thrown if initialization fails
 */
struct OCXInitializationFailedException : OCXBaseException {
  using OCXBaseException::OCXBaseException;
};

/**
 * Basic NotFound exception
 */
struct OCXNotFoundException : OCXBaseException {
  using OCXBaseException::OCXBaseException;
};

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_EXCEPTIONS_H_
