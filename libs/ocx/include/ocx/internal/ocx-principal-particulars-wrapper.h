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
#include <optional>

namespace ocx::context_entities {

/**
 * @brief Enumerated free-board types according to the Rules
 */
enum class FreeboardType {
  /**
   * Type 'A' ship is one which:
   * \verbatim
   * — is designed to carry only liquid cargoes in bulk
   * — has a high integrity of the exposed deck with only small access openings
   * to cargo compartments, closed by watertight gasket covers of steel or
   * equivalent material
   * — has low permeability of loaded cargo compartments.
   * \endverbatim
   */
  A,
  /**
   * All ships which are not Type 'A' ships shall be considered as Type 'B'
   * ships.
   */
  B,
};

/**
 * @brief PrincipalParticularsWrapper is used to store the information of a
 * principal particulars node.
 */
struct PrincipalParticularsWrapper {
  /**
   * Construct a default PrincipalParticularsWrapper.
   */
  PrincipalParticularsWrapper() = default;

  /**
   * The length of the Vessel between perpendiculars, Lpp.
   */
  std::optional<Standard_Real> m_lpp;

  /**
   * Rule (scantling) length, L. A length measurement for the ship that is
   * defined in classification society rules (see ISO 10303-218,
   * section 4.2.32.4).
   */
  std::optional<Standard_Real> m_ruleLength;

  /**
   * The ratio of the moulded displacement volume to the volume of a block that
   * has its length equal to the length class, its breadth equal to the moulded
   * breadth and its depth equal to the scantlings draught (see ISO 10303-218,
   * section 4.2.32.1).
   */
  std::optional<Standard_Real> m_blockCoefficient;

  /**
   * X-Position of fwd. end of waterline for free-board length.
   */
  std::optional<Standard_Real> m_fpPos;

  /**
   * The moulded breadth of the Vessel, B.
   */
  std::optional<Standard_Real> m_mouldedBreath;

  /**
   * The moulded depth of the Vessel, D.
   */
  std::optional<Standard_Real> m_mouldedDepth;

  /**
   * Design draught moulded, fully loaded condition, Td. The summer load draught
   * used by the classification society in its calculations for structural
   * integrity and strength (see ISO 10303-218, section 4.2.32.6).
   */
  std::optional<Standard_Real> m_scantlingDraught;

  /**
   * The forward or service speed at which the ship is designed to operate (see
   * ISO 10303-218, section 4.2.32.2).
   */
  std::optional<Standard_Real> m_designSpeed;

  /**
   * The free-board length of the Vessel, Lll.
   */
  std::optional<Standard_Real> m_freeboardLength;

  /**
   * The Vessel draught at normal ballast, Tnb.
   */
  std::optional<Standard_Real> m_normalBallastDraught;

  /**
   * The Vessel draught at heavy ballast, Thb.
   */
  std::optional<Standard_Real> m_heavyBallastDraught;

  /**
   * The design slamming draught at FP (all ballast tanks empty), Tf-e.
   */
  std::optional<Standard_Real> m_slammingDraughtEmptyFP;

  /**
   * The Vessel draught at FP used when calculation design slamming loads (all
   * ballast tanks full), Tf-f.
   */
  std::optional<Standard_Real> m_slammingDraughtFullFP;

  /**
   * The length of the waterline at T, Lwl.
   */
  std::optional<Standard_Real> m_lengthOfWaterline;

  /**
   * The height of free-board deck, D1.
   */
  std::optional<Standard_Real> m_freeboardDeckHeight;

  /**
   * X Position of AP.
   */
  std::optional<Standard_Real> m_apPos;

  /**
   * Z coordinate of the bulkhead deck.
   */
  std::optional<Standard_Real> m_zPosOfDeck;

  /**
   * Deepest equilibrium waterline in damaged condition.
   */
  std::optional<Standard_Real> m_deepestEquilibriumWL;

  /**
   * Projected area of upper deck forward 0.2 L.
   */
  std::optional<Standard_Real> m_upperDeckArea;

  /**
   * The area of water-plane forward 0.2 L at scantling draught Td.
   */
  std::optional<Standard_Real> m_waterPlaneArea;

  /**
   * Vertical distance from baseline to deck-line at FE.
   */
  std::optional<Standard_Real> m_zPosDeckline;

  /**
   * Speed factor Cav.
   */
  std::optional<Standard_Real> m_speedFactor;

  /**
   * The ship has dead-weight less than 50000 tonnes (boolean).
   */
  std::optional<bool> m_hasDeadweightLessThan;

  /**
   * Whether the vessel has a bilge keel or not.
   */
  std::optional<bool> m_hasBilgeKeel;

  /**
   * Enumerated free-board types according to the Rules.
   */
  std::optional<FreeboardType> m_freeboardType;

  /**
   * Number of decks above 0.7 D from baseline.
   */
  std::optional<int> m_numberOfDecksAbove;
};

}  // namespace ocx::context_entities

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_PRINCIPAL_PARTICULAR_WRAPPER_H_
