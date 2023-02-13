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

#include "ocx/internal/ocx-refplane-wrapper.h"

#include <LDOM_Element.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <string>

namespace ocx::context_entities {

RefPlaneWrapper::RefPlaneWrapper() : m_type(RefPlaneType::UNDEF) {}

RefPlaneWrapper::RefPlaneWrapper(std::string_view id, RefPlaneType const &type,
                                 LDOM_Element const &refPlaneN,
                                 gp_Dir const &normal, gp_Pnt const &p1,
                                 gp_Pnt const &p2, gp_Pnt const &p3)
    : m_id(id),
      m_type(type),
      m_refPlaneN(refPlaneN),
      m_normal(normal),
      m_p1(p1),
      m_p2(p2),
      m_p3(p3) {}

}  // namespace ocx::context_entities
