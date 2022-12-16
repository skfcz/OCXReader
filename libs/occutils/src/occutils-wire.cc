#include "occutils/occutils-wire.h"

#include <BRepLib_MakeWire.hxx>
#include <stdexcept>

#include "occutils/occutils-edge.h"
#include "occutils/occutils-equality.h"
#include "occutils/occutils-face.h"
#include "occutils/occutils-pipe.h"
#include "occutils/occutils-point.h"

namespace OCCUtils::Wire {

TopoDS_Wire FromEdges(const std::initializer_list<TopoDS_Edge>& edges) {
  BRepLib_MakeWire wireMaker;
  for (const auto& edge : edges) {
    if (edge.IsNull()) {
      continue;
    }
    wireMaker.Add(edge);
  }
  return wireMaker.IsDone() ? wireMaker.Wire() : TopoDS_Wire();
}

TopoDS_Wire FromEdges(const std::vector<TopoDS_Edge>& edges) {
  BRepLib_MakeWire wireMaker;
  for (const auto& edge : edges) {
    if (edge.IsNull()) {
      continue;
    }
    wireMaker.Add(edge);
  }
  return wireMaker.IsDone() ? wireMaker.Wire() : TopoDS_Wire();
}

IncrementalBuilder::IncrementalBuilder(const gp_Pnt& pnt)
    : current(pnt), edges() {
  currentDirection = std::nullopt;
  edges.reserve(
      25);  // Prevent frequent reallocation's at the expense of some memory
}

/**
 * Add a line segment
 */
void IncrementalBuilder::Line(double dx, double dy, double dz) {
  gp_Pnt p1(current);  // Copy current point
  // Increment coordinates
  current = current + gp_Pnt(dx, dy, dz);
  // Create edges
  currentDirection = gp_Vec(p1, current);
  edges.emplace_back(Edge::FromPoints(p1, current));
}

void IncrementalBuilder::Arc90(double dx, double dy, double dz, double centerDx,
                               double centerDy, double centerDz,
                               const gp_Dir& normal) {
  gp_Pnt p2 = current + gp_Pnt(dx, dy, dz);
  gp_Pnt center = current + gp_Pnt(centerDx, centerDy, centerDz);
  gp_Dir resultingDirection(gp_Vec(current, center));
  double radius = current.Distance(center);
  double radiusAlt = p2.Distance(center);
  if (abs(radius - radiusAlt) >= Precision::Confusion()) {
    throw std::invalid_argument("dx/dy/dz does not match centerD...!");
  }
  // Current algorithm: Compute both options,
  // one is 90° and one is 270°, select the shorter one.
  auto option1 =
      Edge::CircleSegment(gp_Ax2(center, normal), radius, current, p2);
  auto option2 =
      Edge::CircleSegment(gp_Ax2(center, normal), radius, p2, current);
  double length1 = Edge::Length(option1);
  double length2 = Edge::Length(option2);
  edges.emplace_back(length1 < length2 ? option1 : option2);
  current = p2;
  currentDirection = resultingDirection;
}

TopoDS_Wire IncrementalBuilder::Wire() const { return Wire::FromEdges(edges); }

gp_Pnt IncrementalBuilder::Location() {
  return {current};  // make copy to avoid modification
}

/**
 * Create a pipe from the wire using the given profile.
 */
TopoDS_Shape IncrementalBuilder::Pipe(const TopoDS_Face& profile) const {
  return Pipe::FromSplineAndProfile(this->Wire(), profile);
}

TopoDS_Shape IncrementalBuilder::PipeWithCircularProfile(double radius) {
  auto profile = Face::FromEdge(Edge::FullCircle(
      gp_Ax2(current, currentDirection.value_or(Direction::Z())), radius));
  return Pipe(profile);
}

std::optional<gp_Dir> IncrementalBuilder::Direction() {
  // Make copy of direction to prevent modification!
  if (currentDirection.has_value()) {
    return std::make_optional(gp_Dir(currentDirection.value()));
  } else {
    return std::nullopt;
  }
}

TopoDS_Wire FromPoints(const std::vector<gp_Pnt>& points, bool close) {
  if (points.size() < 2) {
    return {};
  }
  // Build directly without making a vector of edges
  // This is likely slightly more efficient
  BRepLib_MakeWire makeWire;
  for (size_t i = 0; i < points.size() - 1; i++) {
    const auto& p1 = points[i];
    const auto& p2 = points[i + 1];
    // Ignore duplicate points
    if (p1 == p2) {
      continue;
    }
    makeWire.Add(Edge::FromPoints(p1, p2));
  }
  // Close curve if enabled
  if (close) {
    const auto& p0 = points[0];
    const auto& plast = points[points.size() - 1];
    if (p0 != plast) {
      makeWire.Add(Edge::FromPoints(p0, plast));
    }
  }
  return makeWire.Wire();
}

}  // namespace OCCUtils::Wire
