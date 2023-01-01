/***************************************************************************
 *   Created on: 01 Dec 2022                                               *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PANEL_H_
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PANEL_H_

#include <list>
#include <string>
#include <vector>

#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-entity-with-properties.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-extrusion.h"
#include "shipxml/internal/shipxml-limit.h"
#include "shipxml/internal/shipxml-plate.h"
#include "shipxml/internal/shipxml-support.h"

namespace shipxml {

class Panel : public EntityWithProperties {
 public:
  /**
   * Create new Panel Objects
   * @param name the name of the panel
   */
  explicit Panel(std::string const& name);

  void SetBlockName(std::string_view blockName);

  [[nodiscard]] std::string GetBlockName() const;

  void SetCategory(std::string_view category);

  [[nodiscard]] std::string GetCategory() const;

  void SetCategoryDescription(std::string_view categoryDescription);

  [[nodiscard]] std::string GetCategoryDescription() const;

  void SetPlanar(bool isPlanar);

  [[nodiscard]] bool IsPlanar() const;

  void SetPillar(bool isPillar);

  [[nodiscard]] bool IsPillar() const;

  void SetOwner(std::string_view owner);

  [[nodiscard]] std::string GetOwner() const;

  void SetDefaultMaterial(std::string_view defaultMaterial);

  [[nodiscard]] std::string GetDefaultMaterial() const;

  void SetTightness(std::string_view tightness);

  [[nodiscard]] std::string GetTightness() const;

  /**
   * Get the Extrusion
   */
  [[nodiscard]] Extrusion GetExtrusion() const;

  /**
   * Get the Support
   */
  [[nodiscard]] shipxml::Support GetSupport() const;


  /**
   * Set the Support
   */
  void SetSupport(shipxml::Support const& support);

  /**
   * Set the list of Limits
   * @param list the list of Limits
   */
  void SetLimits(std::list<shipxml::Limit> const& limits);

  /**
   * Get the list of Limits
   * @return the list of Limits
   */
  [[nodiscard]] std::list<shipxml::Limit> GetLimits() const;

  /**
   * Set the boundary curve
   */
  void SetGeometry(shipxml::AMCurve const& geometry);

  /**
   * Get the boundary curve (may be null)
   */
  [[nodiscard]] shipxml::AMCurve GetGeometry() const;

  /**
   * Get the list of Plates
   * @return the list of Plates
   */
  [[nodiscard]] std::vector<shipxml::Plate>&   GetPlates();



 private:
  std::string m_blockName;
  std::string m_category;
  std::string m_categoryDescription;
  bool m_planar = false;
  bool m_pillar = false;
  std::string m_owner;
  std::string m_defaultMaterial;
  std::string m_tightness;
  shipxml::Extrusion m_extrusion;

  shipxml::Support m_support;
  std::list<shipxml::Limit> m_limits;
  shipxml::AMCurve m_geometry;
  std::vector<shipxml::Plate> m_plates;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PANEL_H_
