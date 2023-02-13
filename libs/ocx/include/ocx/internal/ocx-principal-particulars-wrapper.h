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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_PRINCIPAL_PARTICULAR_WRAPPER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_PRINCIPAL_PARTICULAR_WRAPPER_H_

#include <Standard_Real.hxx>

namespace ocx::context_entities {

/**
 * @brief PrincipalParticularsWrapper is used to store the information of a
 * principal particulars node.
 */
struct PrincipalParticularsWrapper {
  Standard_Real m_lpp;
  Standard_Real m_ruleLength;
  Standard_Real m_blockCoefficient;
  Standard_Real m_fpPos;
  Standard_Real m_mouldedBreath;
  Standard_Real m_mouldedDepth;
  Standard_Real m_scantlingDraught;
  Standard_Real m_designSpeed;
  Standard_Real m_apPos;

  /**
   * Construct a default PrincipalParticularsWrapper.
   */
  PrincipalParticularsWrapper() = default;
};

}  // namespace ocx::context_entities

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_PRINCIPAL_PARTICULAR_WRAPPER_H_
