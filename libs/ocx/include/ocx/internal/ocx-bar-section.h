/***************************************************************************
 *   Created on: 29 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_BAR_SECTION_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_BAR_SECTION_H_

#include <Standard_Real.hxx>
#include <string>

namespace ocx::context_entities {

class BarSection {
 public:
  BarSection() = default;
  virtual ~BarSection() = default;

  explicit BarSection(std::string barSectionType);

  [[nodiscard]] std::string GetBarSectionType() const;

 private:
  std::string m_barSectionType;
};

//-----------------------------------------------------------------------------

class FlatBar final : public BarSection {
 public:
  FlatBar(std::string const &barSectionType, Standard_Real width,
          Standard_Real height);
  ~FlatBar() override = default;

  [[nodiscard]] Standard_Real GetWidth() const;
  [[nodiscard]] Standard_Real GetHeight() const;

 private:
  Standard_Real m_width;
  Standard_Real m_height;
};

//-----------------------------------------------------------------------------

class RoundBar final : public BarSection {
 public:
  RoundBar(std::string const &barSectionType, Standard_Real height,
           Standard_Real diameter);
  ~RoundBar() override = default;

  [[nodiscard]] Standard_Real GetHeight() const;
  [[nodiscard]] Standard_Real GetDiameter() const;

 private:
  Standard_Real m_height;
  Standard_Real m_diameter;
};

}  // namespace ocx::context_entities

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_BAR_SECTION_H_
