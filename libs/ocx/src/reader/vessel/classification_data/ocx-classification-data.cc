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

#include "ocx/internal/ocx-classification-data.h"

#include <LDOM_Element.hxx>

#include "ocx/internal/ocx-principal-particulars-wrapper.h"
#include "ocx/ocx-context.h"

namespace ocx::reader::vessel::classification_data {

void ReadClassificationData(LDOM_Element const &vesselN) {
  OCX_INFO("Reading classification data")

  LDOM_Element classificationDataN =
      ocx::helper::GetFirstChild(vesselN, "ClassificationData");
  if (classificationDataN.isNull()) {
    OCX_WARN("No ClassificationData child node found.")
    return;
  }

  // Read principal particulars
  ReadPrincipalParticulars(classificationDataN);
}

//-----------------------------------------------------------------------------

namespace {

void ReadPrincipalParticulars(LDOM_Element const &classificationDataN) {
  OCX_INFO("Reading principal particular...")

  LDOM_Element principalParticularsN =
      ocx::helper::GetFirstChild(classificationDataN, "PrincipalParticulars");
  if (principalParticularsN.isNull()) {
    OCX_WARN("No PrincipalParticulars node found")
    return;
  }

  // Read principal particular properties
  ocx::context_entities::PrincipalParticularsWrapper
      principalParticularsWrapper{};

  if (LDOM_Element lppN =
          ocx::helper::GetFirstChild(principalParticularsN, "Lpp");
      !lppN.isNull()) {
    principalParticularsWrapper.m_lpp = ocx::helper::ReadDimension(lppN);
  }

  if (LDOM_Element ruleLengthN =
          ocx::helper::GetFirstChild(principalParticularsN, "RuleLength");
      !ruleLengthN.isNull()) {
    principalParticularsWrapper.m_ruleLength =
        ocx::helper::ReadDimension(ruleLengthN);
  }

  if (LDOM_Element blockCoefficientN =
          ocx::helper::GetFirstChild(principalParticularsN, "BlockCoefficient");
      !blockCoefficientN.isNull()) {
    principalParticularsWrapper.m_blockCoefficient =
        ocx::helper::ReadDimension(blockCoefficientN);
  }

  if (LDOM_Element fpPosN =
          ocx::helper::GetFirstChild(principalParticularsN, "FP_Pos");
      !fpPosN.isNull()) {
    principalParticularsWrapper.m_fpPos = ocx::helper::ReadDimension(fpPosN);
  }

  if (LDOM_Element mouldedBreadthN =
          ocx::helper::GetFirstChild(principalParticularsN, "MouldedBreadth");
      !mouldedBreadthN.isNull()) {
    principalParticularsWrapper.m_mouldedBreath =
        ocx::helper::ReadDimension(mouldedBreadthN);
  }

  if (LDOM_Element mouldedDepthN =
          ocx::helper::GetFirstChild(principalParticularsN, "MouldedDepth");
      !mouldedDepthN.isNull()) {
    principalParticularsWrapper.m_mouldedDepth =
        ocx::helper::ReadDimension(mouldedDepthN);
  }

  if (LDOM_Element scantlingDraughtN =
          ocx::helper::GetFirstChild(principalParticularsN, "ScantlingDraught");
      !scantlingDraughtN.isNull()) {
    principalParticularsWrapper.m_scantlingDraught =
        ocx::helper::ReadDimension(scantlingDraughtN);
  }

  if (LDOM_Element designSpeedN =
          ocx::helper::GetFirstChild(principalParticularsN, "DesignSpeed");
      !designSpeedN.isNull()) {
    principalParticularsWrapper.m_designSpeed =
        ocx::helper::ReadDimension(designSpeedN);
  }

  if (LDOM_Element freeboardLengthN =
          ocx::helper::GetFirstChild(principalParticularsN, "FreeboardLength");
      !freeboardLengthN.isNull()) {
    principalParticularsWrapper.m_freeboardLength =
        ocx::helper::ReadDimension(freeboardLengthN);
  }

  if (LDOM_Element normalBallastDraughtN = ocx::helper::GetFirstChild(
          principalParticularsN, "NormalBallastDraught");
      !normalBallastDraughtN.isNull()) {
    principalParticularsWrapper.m_normalBallastDraught =
        ocx::helper::ReadDimension(normalBallastDraughtN);
  }

  if (LDOM_Element heavyBallastDraughtN = ocx::helper::GetFirstChild(
          principalParticularsN, "HeavyBallastDraught");
      !heavyBallastDraughtN.isNull()) {
    principalParticularsWrapper.m_heavyBallastDraught =
        ocx::helper::ReadDimension(heavyBallastDraughtN);
  }

  if (LDOM_Element slammingDraughtEmptyFPN = ocx::helper::GetFirstChild(
          principalParticularsN, "SlammingDraughtEmptyFP");
      !slammingDraughtEmptyFPN.isNull()) {
    principalParticularsWrapper.m_slammingDraughtEmptyFP =
        ocx::helper::ReadDimension(slammingDraughtEmptyFPN);
  }

  if (LDOM_Element slammingDraughtFullFPN = ocx::helper::GetFirstChild(
          principalParticularsN, "SlammingDraughtFullFP");
      !slammingDraughtFullFPN.isNull()) {
    principalParticularsWrapper.m_slammingDraughtFullFP =
        ocx::helper::ReadDimension(slammingDraughtFullFPN);
  }

  if (LDOM_Element lengthOfWaterlineN = ocx::helper::GetFirstChild(
          principalParticularsN, "LengthOfWaterline");
      !lengthOfWaterlineN.isNull()) {
    principalParticularsWrapper.m_lengthOfWaterline =
        ocx::helper::ReadDimension(lengthOfWaterlineN);
  }

  if (LDOM_Element freeboardDeckHeightN = ocx::helper::GetFirstChild(
          principalParticularsN, "FreeboardDeckHeight");
      !freeboardDeckHeightN.isNull()) {
    principalParticularsWrapper.m_freeboardDeckHeight =
        ocx::helper::ReadDimension(freeboardDeckHeightN);
  }

  if (LDOM_Element apPosN =
          ocx::helper::GetFirstChild(principalParticularsN, "AP_Pos");
      !apPosN.isNull()) {
    principalParticularsWrapper.m_apPos = ocx::helper::ReadDimension(apPosN);
  }

  if (LDOM_Element deepestEquilibriumWLN = ocx::helper::GetFirstChild(
          principalParticularsN, "DeepestEquilibriumWL");
      !deepestEquilibriumWLN.isNull()) {
    principalParticularsWrapper.m_deepestEquilibriumWL =
        ocx::helper::ReadDimension(deepestEquilibriumWLN);
  }

  if (LDOM_Element upperDeckAreaN =
          ocx::helper::GetFirstChild(principalParticularsN, "UpperDeckArea");
      !upperDeckAreaN.isNull()) {
    principalParticularsWrapper.m_upperDeckArea =
        ocx::helper::ReadDimension(upperDeckAreaN);
  }

  if (LDOM_Element waterPlaneAreaN =
          ocx::helper::GetFirstChild(principalParticularsN, "WaterPlaneArea");
      !waterPlaneAreaN.isNull()) {
    principalParticularsWrapper.m_waterPlaneArea =
        ocx::helper::ReadDimension(waterPlaneAreaN);
  }

  if (LDOM_Element zPosDecklineN =
          ocx::helper::GetFirstChild(principalParticularsN, "ZPosDeckline");
      !zPosDecklineN.isNull()) {
    principalParticularsWrapper.m_zPosDeckline =
        ocx::helper::ReadDimension(zPosDecklineN);
  }

  if (LDOM_Element speedFactorN =
          ocx::helper::GetFirstChild(principalParticularsN, "SpeedFactor");
      !speedFactorN.isNull()) {
    principalParticularsWrapper.m_speedFactor =
        ocx::helper::ReadDimension(speedFactorN);
  }

  // TODO: Add support for the following principal particulars properties:
  // TODO: - hasDeadweightLessThan
  // TODO: - hasBilgeKeel
  // TODO: - freeboardType
  // TODO: - numberOfDecksAbove

  // Add principal particular to context
  ocx::OCXContext::GetInstance()->RegisterPrincipalParticulars(
      principalParticularsWrapper);
}

}  // namespace

}  // namespace ocx::reader::vessel::classification_data
