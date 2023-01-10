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

#include "shipxml-plate.h"
#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-entity-with-properties.h"
#include "shipxml/internal/shipxml-enums.h"
#include "shipxml/internal/shipxml-extrusion.h"
#include "shipxml/internal/shipxml-limit.h"
#include "shipxml/internal/shipxml-support.h"

namespace shipxml {

class Panel : public EntityWithProperties {
 public:
  /**
   * Create new Panel Objects
   * @param name the name of the panel
   */
  explicit Panel(std::string_view name);

  void SetBlockName(std::string_view blockName);
  [[nodiscard]] std::string GetBlockName() const;

  void SetCategory(std::string_view category);
  [[nodiscard]] std::string GetCategory() const;

  void SetCategoryDescription(std::string_view categoryDescription);
  [[nodiscard]] std::string GetCategoryDescription() const;

  void SetIsPlanar(bool isPlanar);
  [[nodiscard]] bool IsPlanar() const;

  void SetIsPillar(bool isPillar);
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
   * Set the Support
   */
  void SetSupport(shipxml::Support const& support);
  
  /**
   * Get the Support
   */
  [[nodiscard]] Support GetSupport() const;

  /**
   * Set the list of Limits
   * @param list the list of Limits
   */
  void SetLimits(std::list<Limit> const& limits);

  /**
   * Get the list of Limits
   * @return the list of Limits
   */
  [[nodiscard]] std::list<Limit> GetLimits() const;

  /**
   * Set the boundary curve
   */
  void SetGeometry(AMCurve const& geometry);

  /**
   * Get the boundary curve (may be null)
   */
  [[nodiscard]] AMCurve GetGeometry() const;

  /**
   * Get a vector of Plates
   *
   * @return the vector of Plates
   */
  [[nodiscard]] std::vector<Plate> GetPlates() const;

 private:
  std::string m_blockName;
  std::string m_category;
  std::string m_categoryDescription;
  bool m_planar = false;
  bool m_pillar = false;
  std::string m_owner;
  std::string m_defaultMaterial;
  std::string m_tightness;
  Extrusion m_extrusion;

  Support m_support;
  std::list<Limit> m_limits;
  AMCurve m_geometry{AMSystem::XY};
  std::vector<Plate> m_plates;
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PANEL_H_
