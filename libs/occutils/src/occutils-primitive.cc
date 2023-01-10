#include "occutils/occutils-primitive.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include "occutils/occutils-direction.h"
#include "occutils/occutils-point.h"

namespace OCCUtils::Primitive {

TopoDS_Solid MakeBox(double xSize, double ySize, double zSize, int center,
                     gp_Pnt origin) {
  // Compute offsets based on centering
  if (center & (int)PositionCentering::CenterX) {
    origin.SetX(origin.X() - xSize / 2.0);
  }
  if (center & (int)PositionCentering::CenterY) {
    origin.SetY(origin.Y() - ySize / 2.0);
  }
  if (center & (int)PositionCentering::CenterZ) {
    origin.SetZ(origin.Z() - zSize / 2.0);
  }
  // Build primitive
  BRepPrimAPI_MakeBox box(origin, xSize, ySize, zSize);
  box.Build();
  return box.Solid();
}

TopoDS_Solid MakeBox(const gp_Pnt& a, const gp_Pnt& b) {
  BRepPrimAPI_MakeBox box(a, b.X() - a.X(), b.Y() - a.Y(), b.Z() - a.Z());
  box.Build();
  return box.Solid();
}

TopoDS_Solid MakeBox(const gp_Vec& a, const gp_Vec& b) {
  return MakeBox(gp_Pnt(a.X(), a.Y(), a.Z()), gp_Pnt(b.X(), b.Y(), b.Z()));
}

TopoDS_Solid MakeBox(const std::pair<gp_Pnt, gp_Pnt>& ab) {
  return MakeBox(ab.first, ab.second);
}

TopoDS_Solid MakeBox(const std::pair<gp_Vec, gp_Vec>& ab) {
  return MakeBox(ab.first, ab.second);
}

TopoDS_Solid MakeCube(double size, int center, gp_Pnt origin) {
  return MakeBox(size, size, size, center, origin);
}

TopoDS_Solid MakeCone(const gp_Ax1& axis, double diameter1, double diameter2,
                      double length, bool centerLength) {
  BRepPrimAPI_MakeCone builder(
      gp_Ax2(centerLength ? axis.Location() - axis.Direction() * (length / 2)
                          : axis.Location(),
             axis.Direction()),
      diameter1, diameter2, length);
  return builder.Solid();
}

TopoDS_Solid MakeCylinder(double diameter, double length,
                          Orientation orientation, int center, gp_Pnt origin) {
  // Compute offsets based on centering
  if (center & (int)PositionCentering::CenterL) {
    if (orientation == Orientation::X) {
      origin.SetX(origin.X() - diameter / 2.0);
    } else if (orientation == Orientation::Y) {
      origin.SetY(origin.Y() - diameter / 2.0);
    } else if (orientation == Orientation::Z) {
      origin.SetZ(origin.Z() - diameter / 2.0);
    }
  }
  // Which axis
  gp_Dir axis{};
  if (orientation == Orientation::X) {
    axis = Direction::X();
  } else if (orientation == Orientation::Y) {
    axis = Direction::Y();
  } else if (orientation == Orientation::Z) {
    axis = Direction::Z();
  }
  // Build primitive
  gp_Ax2 ax(origin, axis);
  BRepPrimAPI_MakeCylinder cyl(ax, diameter / 2.0, length);
  cyl.Build();
  return cyl.Solid();
}

}  // namespace OCCUtils::Primitive
