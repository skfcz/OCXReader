#include "occutils/occutils-axis.h"

#include <gp_Lin.hxx>

#include "occutils/occutils-direction.h"
#include "occutils/occutils-point.h"

namespace OCCUtils {

namespace Axis {

bool Contains(const gp_Ax1& axis, const gp_Pnt& pnt, double tolerance) {
  return gp_Lin(axis).Contains(pnt, tolerance);
}

double Distance(const gp_Ax1& axis, const gp_Pnt& pnt) {
  return gp_Lin(axis).Distance(pnt);
}

gp_Ax1 operator+(const gp_Ax1& axis, const gp_Vec& vec) {
  return {axis.Location() + vec, axis.Direction()};
}

gp_Ax1 operator+(const gp_Ax1& axis, const gp_XYZ& vec) {
  return {axis.Location() + vec, axis.Direction()};
}

}  // namespace Axis

namespace Ax1 {

gp_Ax1 OX() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::X()}; }

gp_Ax1 OY() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::Y()}; }

gp_Ax1 OZ() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::Z()}; }

gp_Ax1 OMinusX() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusX()};
}

gp_Ax1 OMinusY() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusY()};
}

gp_Ax1 OMinusZ() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusZ()};
}

}  // namespace Ax1

namespace Ax2 {

gp_Ax2 FromAx1(const gp_Ax1& axis) {
  return {axis.Location(), axis.Direction()};
}

gp_Ax2 OX() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::X()}; }

gp_Ax2 OY() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::Y()}; }

gp_Ax2 OZ() { return {OCCUtils::Point::Origin(), OCCUtils::Direction::Z()}; }

gp_Ax2 OMinusX() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusX()};
}

gp_Ax2 OMinusY() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusY()};
}

gp_Ax2 OMinusZ() {
  return {OCCUtils::Point::Origin(), OCCUtils::Direction::MinusZ()};
}

}  // namespace Ax2

}  // namespace OCCUtils
