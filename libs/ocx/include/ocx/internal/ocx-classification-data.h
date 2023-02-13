/***************************************************************************
 *   Created on: 13 Feb 2023                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_CLASSIFICATION_DATA_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_CLASSIFICATION_DATA_H_

#include <LDOM_Element.hxx>

namespace ocx::reader::vessel::classification_data {

void ReadClassificationData(LDOM_Element const &vesselN);

namespace {  // anonymous namespace

void ReadPrincipalParticulars(LDOM_Element const &classificationDataN);

}  // namespace

}  // namespace ocx::reader::vessel::classification_data

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_CLASSIFICATION_DATA_H_
