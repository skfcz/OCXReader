/***************************************************************************
 *   Created on: 16 Feb 2023                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_VESSEL_GRID_WRAPPER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_VESSEL_GRID_WRAPPER_H_

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <optional>

namespace ocx::context_entities {

/**
 * VesselGridType is used to store the type of a vessel grid.
 */
enum class VesselGridType {
  /**
   * The definition of the vessel X grid system
   */
  XGrid,
  /**
   * The definition of the vessel Y grid system
   */
  YGrid,
  /**
   * The definition of the vessel Z grid system
   */
  ZGrid
};

/**
 * SpacingGroup is used to store the information of a spacing group.
 */
struct SpacingGroup {
  /**
   * Default constructor is deleted. Prohibit instantiation without defined
   * values.
   */
  SpacingGroup() = delete;

  /**
   * Construct a spacing group with the given values.
   *
   * @param firstGridNumber The first grid number in this group.
   * @param count Number of repeated grid spacings in this group.
   * @param spacing The grid spacing of the spacing group.
   * @param gridPosition The location of the first grid position in the spacing
   * group.
   */
  SpacingGroup(Standard_Integer const &firstGridNumber,
               Standard_Integer const &count, Standard_Real const &spacing,
               Standard_Real const &gridPosition);

  /**
   * The first grid number in this group.
   */
  Standard_Integer m_firstGridNumber;

  /**
   * Number of repeated grid spacings in this group.
   */
  Standard_Integer m_count;

  /**
   * The grid spacing of the spacing group.
   */
  Standard_Real m_spacing;

  /**
   * The location of the first grid position in the spacing group.
   */
  Standard_Real m_gridPosition;
};

/**
 * VesselGridWrapper is used to store the information of a vessel grid.
 */
class VesselGridWrapper {
 public:
  /**
   * Default constructor is deleted. Prohibit instantiation without defined grid
   * type.
   */
  VesselGridWrapper() = delete;

  /**
   * Construct a default VesselGridWrapper.
   *
   * @param type The Vessel coordinate system grid type
   */
  explicit VesselGridWrapper(VesselGridType const &type);

  void SetVesselGridType(VesselGridType const &type);
  [[nodiscard]] VesselGridType GetVesselGridType() const;

  void SetDistanceToAp(Standard_Real const &distanceToAp);
  [[nodiscard]] std::optional<Standard_Real> GetDistanceToAp() const;

  void AddSpacingGroup(SpacingGroup const &spacingGroup);
  [[nodiscard]] std::vector<SpacingGroup> GetSpacingGroups() const;

  void SetIsReversed(bool const &isReversed);
  [[nodiscard]] bool GetIsReversed() const;

  void SetIsMainSystem(bool const &isMainSystem);
  [[nodiscard]] bool GetIsMainSystem() const;

 private:
  /**
   * The Vessel coordinate system grid type
   */
  VesselGridType m_type;

  /**
   * If #0/FR0 (FR0 has per definition ReferenceLocation=0) is not located at
   * AP, give the offset here (positive or negative distance).
   */
  std::optional<Standard_Real> m_distanceToAp;

  /**
   * The spacing groups of the frame table of the vessel grid.
   */
  std::vector<SpacingGroup> m_spacingGroups;

  /**
   * Whether the frame table is reversed with the origin at the fore of the
   * vessel.
   */
  bool m_isReversed = false;

  /**
   * Whether the frame table defines the main vessel grid.
   */
  bool m_isMainSystem = true;
};

}  // namespace ocx::context_entities

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_VESSEL_GRID_WRAPPER_H_
