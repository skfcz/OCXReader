/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

#ifndef OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_
#define OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_

#include <LDOM_Element.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include "ocx/ocx-context.h"

namespace ocx::reader::shared::surface {

/**
 * Base method managing the reading of various surface types, e.g.
 * Cone3D, Cylinder3D, ReadExtrudedSurface, NURBSSurface, Sphere4D, Plane3D
 *
 * @param surfaceN the surface element
 * @return the created TopoDS_Shape or TopoDS_Shell
 */
[[nodiscard]] TopoDS_Shape ReadSurface(LDOM_Element const &surfaceN);

namespace {  // anonymous namespace
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
    LDOM_Element const &surfaceColN);

/**
 * Read a Cone3D from given surface element
 *
 * @param surfaceN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Shape
 */
[[nodiscard]] TopoDS_Face ReadCone3D(LDOM_Element const &surfaceN);

/**
 * Read a Cylinder3D from given surface element
 *
 * @param surfaceN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Face
 */
[[nodiscard]] TopoDS_Face ReadCylinder3D(LDOM_Element const &surfaceN);

/**
 * Read a ExtrudedSurface from given surface element
 *
 * @param surfaceN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Face
 */
[[nodiscard]] TopoDS_Face ReadExtrudedSurface(LDOM_Element const &surfaceN);

/**
 * Read a NURBSSurface from given surface element
 *
 * @param nurbsSrfN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Face
 */
[[nodiscard]] TopoDS_Face ReadNURBSSurface(LDOM_Element const &nurbsSrfN);

/**
 * Read a Sphere3D from given surface element
 *
 * @param surfaceN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Face
 */
[[nodiscard]] TopoDS_Face ReadSphere3D(LDOM_Element const &surfaceN);

/**
 * Read a Plane3D from given surface element
 *
 * @param surfaceN the surface element
 * @param id the id of the surface
 * @param guid the guid of the surface
 * @return the created TopoDS_Face
 */
[[nodiscard]] TopoDS_Face ReadPlane3D(LDOM_Element const &surfaceN);

}  // namespace

}  // namespace ocx::reader::shared::surface

#endif  // OCX_INCLUDE_OCX_INTERNAL_OCX_SURFACE_READER_H_
