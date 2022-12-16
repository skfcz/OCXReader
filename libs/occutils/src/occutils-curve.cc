#include "occutils/occutils-curve.h"

#include <BRep_Tool.hxx>
#include <GCPnts_AbscissaPoint.hxx>

namespace OCCUtils {

namespace Curve {

GeomAdaptor_Curve FromEdge(const TopoDS_Edge& edge) {
  Standard_Real umin, umax;
  // Get unbounded curve plus separate bounding parameters
  auto rawCurve = BRep_Tool::Curve(edge, umin, umax);
  return {rawCurve, umin, umax};
}

GeomAdaptor_Curve FromTrimmedCurve(const Geom_TrimmedCurve& curve) {
  return {curve.BasisCurve(), curve.FirstParameter(), curve.LastParameter()};
}

double Length(const GeomAdaptor_Curve& curve) {
  return GCPnts_AbscissaPoint::Length(curve);
}

double Length(const Handle(Geom_Curve) & curve) {
  return Length(GeomAdaptor_Curve(curve));
}

double Length(const Geom_TrimmedCurve& curve) {
  return Length(FromTrimmedCurve(curve));
}

bool IsLine(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_Line;
}

bool IsCircle(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_Circle;
}

bool IsEllipse(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_Ellipse;
}

bool IsHyperbola(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_Hyperbola;
}

bool IsParabola(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_Parabola;
}

bool IsBezier(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_BezierCurve;
}

bool IsBSpline(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_BSplineCurve;
}

bool IsOffsetCurve(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_OffsetCurve;
}

bool IsOther(const GeomAdaptor_Curve& curve) {
  return curve.GetType() == GeomAbs_OtherCurve;
}

}  // namespace Curve

namespace Curves {

std::vector<GeomAdaptor_Curve> Only(
    const std::vector<GeomAdaptor_Curve>& curves, GeomAbs_CurveType type) {
  return Filter(curves, [type](const GeomAdaptor_Curve& curve) {
    return curve.GetType() == type;
  });
}

/**
 * Filter a list of curves: Get only curves for which the given predicate
 * returns true
 */
std::vector<GeomAdaptor_Curve> Filter(
    const std::vector<GeomAdaptor_Curve>& curves,
    const std::function<bool(const GeomAdaptor_Curve& curve)>& predicate) {
  std::vector<GeomAdaptor_Curve> ret;
  ret.reserve(curves.size());
  for (const auto& curve : curves) {
    if (predicate(curve)) {
      ret.push_back(curve);
    }
  }
  return ret;
}

}  // namespace Curves

}  // namespace OCCUtils
