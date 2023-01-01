/***************************************************************************
*   Created on: 29.12.22.                                                 *
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

#ifndef SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_H
#define SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_H

namespace shipxml {

class Plate : public EntityWithProperties {

 public:
  /**
   * Create new Plate Objects
   * @param name the name of the panel
   */
  explicit Plate(std::string const& name);

  void SetCategory(std::string_view category);

  [[nodiscard]] std::string GetCategory() const;

  void SetCategoryDescription(std::string_view categoryDescription);

  [[nodiscard]] std::string GetCategoryDescription() const;


  void SetMaterial(std::string_view defaultMaterial);

  [[nodiscard]] std::string GetMaterial() const;


  void SetThickness(double t);

  [[nodiscard]] double GetThickness() const;

  void SetOffset(double t);

  [[nodiscard]] double GetOffset() const;

  void SetOrientation( Orientation o);

  [[nodiscard]] Orientation GetOrientation() const;

  /**
   * Set the boundary curve
   */
  void SetGeometry(shipxml::AMCurve const& geometry);

  /**
   * Get the boundary curve (may be null)
   */
  [[nodiscard]] shipxml::AMCurve GetGeometry() const;

  // TODO: holes
  // TODO: seams refs


 private:
  std::string m_category;
  std::string m_categoryDescription;
  std::string m_material;
  double m_thickness;
  double m_offset;
  Orientation m_orientation;
  shipxml::AMCurve m_geometry;

};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_INTERNAL_SHIPXML_PLATE_H
