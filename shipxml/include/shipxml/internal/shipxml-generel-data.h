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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_GENERAL_DATA_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_GENERAL_DATA_H_

#include <string>

namespace shipxml {

class GeneralData {
 public:
  GeneralData() = default;
  ~GeneralData() = default;

  void SetShipName(std::string_view shipName);

  [[nodiscard]] std::string GetShipName() const;

  void SetYardNumber(std::string_view yardNumber);

  [[nodiscard]] std::string GetYardNumber() const;

  void SetAftEnd(double value);

  [[nodiscard]] double GetAftEnd() const;

  void SetAftPerpendicular(double value);

  [[nodiscard]] double GetAftPerpendicular() const;

  void SetForwardEnd(double value);

  [[nodiscard]] double GetForwardEnd() const;

  void SetForwardPerpendicular(double value);

  [[nodiscard]] double GetForwardPerpendicular() const;

  void SetLengthOverall(double value);

  [[nodiscard]] double GetLengthOverall() const;

  void SetMaxDraught(double value);

  [[nodiscard]] double GetMaxDraught() const;

  void SetMaxBreadth(double value);

  [[nodiscard]] double GetMaxBreadth() const;

  void SetMaxHeight(double value);

  [[nodiscard]] double GetMaxHeight() const;

  void SetHeightOfMainDeck(double value);

  [[nodiscard]] double GetHeightOfMainDeck() const;

 private:
  std::string m_shipName;
  std::string m_yardNumber;
  double m_aftEnd{};
  double m_aftPerpendicular{};
  double m_forwardEnd{};
  double m_forwardPerpendicular{};
  double m_lengthOverall{};
  double m_maxDraught{};
  double m_maxBreadth{};
  double m_maxHeight{};
  double m_heightOfMainDeck{};
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_GENERAL_DATA_H_
