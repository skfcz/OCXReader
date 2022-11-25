// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <memory>
#include <string>

#include "ocx/internal/ocx-context.h"

namespace ocx {

class OCXSurfaceReader {
 public:
  /**
   * Create a new surface reader
   *
   * @param ctx the context used to lookup scaling factor
   */
  explicit OCXSurfaceReader(std::shared_ptr<OCXContext> ctx)
      : ctx(std::move(ctx)){};

  /**
   * Base method managing the reading of various surface types, e.g.
   * Cone3D, Cylinder3D, ReadExtrudedSurface, NURBSSurface, Sphere4D, Plane3D
   *
   * @param surfaceN the surface element
   * @return the created TopoDS_Shape or TopoDS_Shell
   */
  [[nodiscard]] TopoDS_Shape ReadSurface(LDOM_Element const &surfaceN) const;

 private:
  /**
   * The context of the reader
   */
  std::shared_ptr<OCXContext> ctx;

  /**
   * Read a combination of Cone3D, Cylinder3D, ReadExtrudedSurface,
   * NURBSSurface, Sphere4D, Plane3D from given surface collection
   *
   * @param surfaceColN the surface collection element
   * @param id the id of the surface collection
   * @param guid the guid of the surface collection
   * @return the created TopoDS_Shape or TopoDS_Shell
   */
  [[nodiscard]] TopoDS_Shape ReadSurfaceCollection(
      LDOM_Element const &surfaceColN, std::string_view id,
      std::string_view guid) const;

  /**
   * Read a Cone3D from given surface element
   *
   * @param surfaceN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Shape
   */
  [[nodiscard]] TopoDS_Face ReadCone3D(LDOM_Element const &surfaceN,
                                       std::string_view id,
                                       std::string_view guid) const;

  /**
   * Read a Cylinder3D from given surface element
   *
   * @param surfaceN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Face
   */
  [[nodiscard]] TopoDS_Face ReadCylinder3D(LDOM_Element const &surfaceN,
                                           std::string_view id,
                                           std::string_view guid) const;

  /**
   * Read a ExtrudedSurface from given surface element
   *
   * @param surfaceN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Face
   */
  [[nodiscard]] TopoDS_Face ReadExtrudedSurface(LDOM_Element const &surfaceN,
                                                std::string_view id,
                                                std::string_view guid) const;

  /**
   * Read a NURBSSurface from given surface element
   *
   * @param nurbsSrfN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Face
   */
  [[nodiscard]] TopoDS_Face ReadNURBSSurface(LDOM_Element const &nurbsSrfN,
                                             std::string_view id,
                                             std::string_view guid) const;

  /**
   * Read a Sphere3D from given surface element
   *
   * @param surfaceN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Face
   */
  [[nodiscard]] TopoDS_Face ReadSphere3D(LDOM_Element const &surfaceN,
                                         std::string_view id,
                                         std::string_view guid) const;

  /**
   * Read a Plane3D from given surface element
   *
   * @param surfaceN the surface element
   * @param id the id of the surface
   * @param guid the guid of the surface
   * @return the created TopoDS_Face
   */
  [[nodiscard]] TopoDS_Face ReadPlane3D(LDOM_Element const &surfaceN,
                                        std::string_view id,
                                        std::string_view guid) const;
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_
