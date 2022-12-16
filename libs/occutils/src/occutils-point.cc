#include "occutils/occutils-point.h"

#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Line.hxx>

#include "occutils/occutils-axis.h"

gp_Pnt operator+(const gp_Pnt &a, const gp_Pnt &b) {
  return {a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z()};
}

gp_Pnt operator+(const gp_Pnt &a, const gp_Vec &b) {
  return {a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z()};
}

gp_Pnt operator+(const gp_Pnt &a, const gp_XYZ &b) {
  return {a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z()};
}

gp_Pnt operator-(const gp_Pnt &a, const gp_Pnt &b) {
  return {a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z()};
}

gp_Pnt operator-(const gp_Pnt &a, const gp_Vec &b) {
  return {a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z()};
}

gp_Pnt operator-(const gp_Pnt &a, const gp_XYZ &b) {
  return {a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z()};
}

namespace OCCUtils::Point {

gp_Pnt Origin() { return {}; }

gp_Pnt Midpoint(const std::initializer_list<gp_Pnt> &points) {
  double x = 0.0, y = 0.0, z = 0.0;
  for (const gp_Pnt &pnt : points) {
    x += pnt.X();
    y += pnt.Y();
    z += pnt.Z();
  }
  size_t size = points.size();
  return {x / size, y / size, z / size};
}

gp_Pnt Midpoint(const std::vector<gp_Pnt> &points) {
  double x = 0.0, y = 0.0, z = 0.0;
  for (const gp_Pnt &pnt : points) {
    x += pnt.X();
    y += pnt.Y();
    z += pnt.Z();
  }
  size_t size = points.size();
  return {x / size, y / size, z / size};
}

double Distance(const gp_Pnt &pnt, const gp_Ax1 &axis) {
  return Axis::Distance(axis, pnt);
}

gp_Pnt OrthogonalProjectOnto(const gp_Pnt &pnt, const gp_Ax1 &ax) {
  Handle(Geom_Line) hax = new Geom_Line(ax);
  auto projector = GeomAPI_ProjectPointOnCurve(pnt, hax);
  projector.Perform(pnt);
  if (projector.NbPoints() == 0) {
    // TODO use more appropriate exception
    throw std::out_of_range("Projection of point onto curve failed");
  }
  return projector.NearestPoint();
}

gp_Pnt2d OrthogonalProjectOnto(const gp_Pnt2d &pnt, const gp_Ax2d &ax) {
  Handle(Geom2d_Line) hax = new Geom2d_Line(ax);
  auto projector = Geom2dAPI_ProjectPointOnCurve(pnt, hax);
  if (projector.NbPoints() == 0) {
    // TODO use more appropriate exception
    throw std::out_of_range("Projection of point onto axis failed");
  }
  return projector.NearestPoint();
}

gp_Pnt From2d(const gp_Pnt2d &pnt) { return {pnt.X(), pnt.Y(), 0.0}; }

gp_Pnt From2d(double x, double y) { return {x, y, 0.0}; }

}  // namespace OCCUtils::Point
