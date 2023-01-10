#pragma once

#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <functional>

namespace OCCUtils::Fillet {

/**
 * Fillet all edges of the given shape using a single radius
 * and return the filleted shape.
 *
 * Usually the shape is a solid.
 */
TopoDS_Shape FilletAll(const TopoDS_Shape& shape, double radius = 1.0);

/**
 * Fillet all edges of the given shape using a
 * user-defined function to define the radius separately
 * for each edge.
 *
 * radiusByEdge takes the edge and should return either:
 *  - A radius
 *  - or NaN if this edge should NOT be filleted
 *
 * Usually the shape is a solid.
 */
TopoDS_Shape FilletAdaptiveRadius(
    const TopoDS_Shape& shape,
    const std::function<double(const TopoDS_Edge&)>& radiusByEdge);

}  // namespace OCCUtils::Fillet
