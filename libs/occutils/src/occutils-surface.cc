#include "occutils/occutils-surface.h"

#include <BRepGProp.hxx>
#include <BRepLib_FindSurface.hxx>
#include <GC_MakeLine.hxx>
#include <GProp_GProps.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>
#include <GeomLProp_SLProps.hxx>
#include <algorithm>
#include <sstream>

#include "occutils/occutils-shape-components.h"

namespace OCCUtils {

namespace Surface {

double Area(const TopoDS_Shape& face) {
  GProp_GProps gprops;
  BRepGProp::SurfaceProperties(face, gprops);
  return gprops.Mass();
}

gp_Pnt CenterOfMass(const TopoDS_Shape& face) {
  GProp_GProps gprops;
  BRepGProp::SurfaceProperties(face, gprops);
  return gprops.CentreOfMass();
}

GeomAdaptor_Surface FromFace(const TopoDS_Face& face) {
  BRepLib_FindSurface bfs(face);
  if (!bfs.Found()) {
    return {};
  }
  return bfs.Surface();
}

std::pair<double, gp_Pnt> AreaAndCenterOfMass(const TopoDS_Shape& face) {
  GProp_GProps gprops;
  BRepGProp::SurfaceProperties(face, gprops);
  return std::make_pair(gprops.Mass(), gprops.CentreOfMass());
}

gp_Ax1 Normal(const GeomAdaptor_Surface& surf, const gp_Pnt2d& uv,
              double precision) {
  return Normal(surf, uv.X(), uv.Y(), precision);
}

gp_Ax1 Normal(const GeomAdaptor_Surface& surf, const gp_XY& uv,
              double precision) {
  return Normal(surf, uv.X(), uv.Y(), precision);
}

gp_Ax1 Normal(const GeomAdaptor_Surface& surf, double u, double v,
              double precision) {
  GeomLProp_SLProps props(surf.Surface(), u, v, 1 /* max 1 derivation */,
                          precision);
  return {props.Value(), props.Normal()};
}

gp_Dir NormalDirection(const GeomAdaptor_Surface& surf, double u, double v,
                       double precision) {
  GeomLProp_SLProps props(surf.Surface(), u, v, 1 /* max 1 derivation */,
                          precision);
  return props.Normal();
}

bool IsPlane(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_Plane;
}

bool IsCylinder(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_Cylinder;
}

bool IsCone(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_Cone;
}

bool IsSphere(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_Sphere;
}

bool IsTorus(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_Torus;
}

bool IsBezierSurface(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_BezierSurface;
}

bool IsBSplineSurface(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_BSplineSurface;
}

bool IsSurfaceOfRevolution(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_SurfaceOfRevolution;
}

bool IsSurfaceOfExtrusion(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_SurfaceOfExtrusion;
}

bool IsOffsetSurface(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_OffsetSurface;
}

bool IsOtherSurface(const GeomAdaptor_Surface& surf) {
  return surf.GetType() == GeomAbs_OtherSurface;
}

/**
 * Sample the point on the given surface at the given U/V coordinates.
 * The point represents the 0th derivative.
 */
gp_Pnt PointAt(const GeomAdaptor_Surface& surf, double u, double v) {
  return surf.Value(u, v);
}

gp_Pnt PointAt(const GeomAdaptor_Surface& surf, const gp_Pnt2d& uv) {
  return surf.Value(uv.X(), uv.Y());
}

gp_Pnt PointAt(const GeomAdaptor_Surface& surf, const gp_XY& uv) {
  return surf.Value(uv.X(), uv.Y());
}

std::vector<gp_XY> UniformUVSampleLocations(const GeomAdaptor_Surface& surf,
                                            size_t uSamples, size_t vSamples) {
  double u0 = surf.FirstUParameter();
  double v0 = surf.FirstVParameter();

  double uInterval = (surf.LastUParameter() - u0) /
                     (uSamples - 1);  // -1: include both end points
  double vInterval = (surf.LastVParameter() - v0) /
                     (vSamples - 1);  // -1: include both end points

  std::vector<gp_XY> ret;
  ret.reserve(uSamples * vSamples);
  for (size_t u = 0; u < uSamples; u++) {
    for (size_t v = 0; v < vSamples; v++) {
      ret.emplace_back(u0 + uInterval * u, v0 + vInterval * v);
    }
  }
  return ret;
}

std::vector<gp_XY> UniformUVSampleLocationsWithin(
    const GeomAdaptor_Surface& surf, size_t uSamples, size_t vSamples) {
  double u0 = surf.FirstUParameter();
  double v0 = surf.FirstVParameter();

  double uInterval = (surf.LastUParameter() - u0) / (uSamples + 1);
  double vInterval = (surf.LastVParameter() - v0) / (vSamples + 1);

  std::vector<gp_XY> ret;
  ret.reserve(uSamples * vSamples);
  for (size_t u = 1; u < uSamples; u++) {
    for (size_t v = 1; v < vSamples; v++) {
      ret.emplace_back(u0 + uInterval * u, v0 + vInterval * v);
    }
  }
  return ret;
}

std::optional<gp_Pnt> Intersection(const gp_Lin& line,
                                   const GeomAdaptor_Surface& surface) {
  GC_MakeLine mkLine(line);
  if (!mkLine.IsDone()) {
    return std::nullopt;
  }
  auto intersector = GeomAPI_IntCS(mkLine.Value(), surface.Surface());
  if (!intersector
           .IsDone()) {  // Algorithm failure, returned as no intersection
    return std::nullopt;
  }
  if (intersector.NbPoints() == 0 || intersector.NbPoints() > 1) {
    return std::nullopt;
  }
  return intersector.Point(1);
}

std::optional<TopoDS_Edge> Intersection(const GeomAdaptor_Surface& S1,
                                        const GeomAdaptor_Surface& S2) {
  auto intersector = GeomAPI_IntSS(S1.Surface(), S2.Surface(),
                                   Precision::Confusion());
  if (!intersector
           .IsDone()) {  // Algorithm failure, returned as no intersection
    return std::nullopt;
  }
  if (intersector.NbLines() == 0 || intersector.NbLines() > 1) {
    return std::nullopt;
  }
  return BRepBuilderAPI_MakeEdge(intersector.Line(1)).Edge();
}

}  // namespace Surface

namespace Surfaces {

std::vector<SurfaceInfo> FromShape(const TopoDS_Shape& shape) {
  auto faces = ShapeComponents::AllFacesWithin(shape);
  // Create return vector
  std::vector<SurfaceInfo> ret;
  ret.reserve(faces.size());
  for (const auto& face : faces) {
    SurfaceInfo info;
    info.face = face;
    info.surface = Surface::FromFace(face);
    if (!info.surface.Surface().IsNull()) {  // If we have found the surface
      ret.push_back(info);
    }
  }
  return ret;
}

std::vector<SurfaceInfo> Only(const std::vector<SurfaceInfo>& surfaces,
                              GeomAbs_SurfaceType type) {
  return Filter(surfaces, [&](const GeomAdaptor_Surface& surf) {
    return surf.GetType() == type;
  });
};

/**
 * Filter surfaces by a custom filter function
 */
std::vector<SurfaceInfo> Filter(
    const std::vector<SurfaceInfo>& surfaces,
    const std::function<bool(const GeomAdaptor_Surface& surf)>& filter) {
  // Create output vector
  std::vector<SurfaceInfo> ret;
  ret.reserve(surfaces.size());
  // Run algorithm
  std::copy_if(surfaces.begin(), surfaces.end(), std::back_inserter(ret),
               [&](const SurfaceInfo& surf) { return filter(surf.surface); });
  return ret;
}

void SurfaceTypeStats::Add(GeomAbs_SurfaceType typ, size_t cnt) {
  if (count.count(typ) == 0) {
    count[typ] = cnt;
  } else {
    count[typ] = count[typ] + cnt;
  }
}

size_t SurfaceTypeStats::Count(GeomAbs_SurfaceType typ) {
  if (count.count(typ) == 0) {
    return 0;
  } else {
    return count[typ];
  }
}

std::string SurfaceTypeStats::Summary() {
  std::ostringstream ss;
  ss << "{\n";
  if (Count(GeomAbs_Plane)) {
    ss << "\tGeomAbs_Plane = " << Count(GeomAbs_Plane) << '\n';
  }
  if (Count(GeomAbs_Cylinder)) {
    ss << "\tGeomAbs_Cylinder = " << Count(GeomAbs_Cylinder) << '\n';
  }
  if (Count(GeomAbs_Cone)) {
    ss << "\tGeomAbs_Cone = " << Count(GeomAbs_Cone) << '\n';
  }
  if (Count(GeomAbs_Sphere)) {
    ss << "\tGeomAbs_Sphere = " << Count(GeomAbs_Sphere) << '\n';
  }
  if (Count(GeomAbs_Torus)) {
    ss << "\tGeomAbs_Torus = " << Count(GeomAbs_Torus) << '\n';
  }
  if (Count(GeomAbs_BezierSurface)) {
    ss << "\tGeomAbs_BezierSurface = " << Count(GeomAbs_BezierSurface) << '\n';
  }
  if (Count(GeomAbs_BSplineSurface)) {
    ss << "\tGeomAbs_BSplineSurface = " << Count(GeomAbs_BSplineSurface)
       << '\n';
  }
  if (Count(GeomAbs_SurfaceOfRevolution)) {
    ss << "\tGeomAbs_SurfaceOfRevolution = "
       << Count(GeomAbs_SurfaceOfRevolution) << '\n';
  }
  if (Count(GeomAbs_SurfaceOfExtrusion)) {
    ss << "\tGeomAbs_SurfaceOfExtrusion = " << Count(GeomAbs_SurfaceOfExtrusion)
       << '\n';
  }
  if (Count(GeomAbs_OffsetSurface)) {
    ss << "\tGeomAbs_OffsetSurface = " << Count(GeomAbs_OffsetSurface) << '\n';
  }
  if (Count(GeomAbs_OtherSurface)) {
    ss << "\tGeomAbs_OtherSurface = " << Count(GeomAbs_OtherSurface) << '\n';
  }
  ss << "}";
  return ss.str();
}

SurfaceTypeStats Statistics(const std::vector<SurfaceInfo>& surfaces) {
  SurfaceTypeStats stats;
  for (const auto& info : surfaces) {
    stats.Add(info.surface.GetType());
  }
  return stats;
}

}  // namespace Surfaces

}  // namespace OCCUtils
