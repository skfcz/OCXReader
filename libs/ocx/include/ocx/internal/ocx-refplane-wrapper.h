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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_REFPLANE_WRAPPER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_REFPLANE_WRAPPER_H_

#include <LDOM_Element.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <string>

namespace ocx::context_entities {

/**
 * RefPlaneType is used to identify the type of a reference plane.
 */
enum class RefPlaneType { X, Y, Z, UNDEF };

/**
 * RefPlaneWrapper is used to store the information of a reference plane.
 */
struct RefPlaneWrapper {
  std::string m_id;
  RefPlaneType m_type;
  LDOM_Element m_refPlaneN;
  gp_Dir m_normal;
  gp_Pnt m_p1;
  gp_Pnt m_p2;
  gp_Pnt m_p3;

  /**
   * Construct a default RefPlaneWrapper object with type UNDEF.
   */
  RefPlaneWrapper();

  /**
   * Construct a RefPlaneWrapper object from given parameters.
   *
   * @param id the id of the reference plane
   * @param type the type of the reference plane
   * @param refPlaneN the reference plane element
   * @param normal the normal of the reference plane
   * @param p1 the first point of the reference plane
   * @param p2 the second point of the reference plane
   * @param p3 the third point of the reference plane
   */
  RefPlaneWrapper(std::string_view id, RefPlaneType const &type,
                  LDOM_Element const &refPlaneN, gp_Dir const &normal,
                  gp_Pnt const &p1, gp_Pnt const &p2, gp_Pnt const &p3);
};

}  // namespace ocx::context_entities

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_REFPLANE_WRAPPER_H_
