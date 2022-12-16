#include "occutils/occutils-fillet.h"

#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <numeric>

namespace OCCUtils::Fillet {

TopoDS_Shape FilletAll(const TopoDS_Shape& shape, double radius) {
  BRepFilletAPI_MakeFillet filletMaker(shape);
  // Iterate edges
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(shape, TopAbs_EDGE, edges);

  for (size_t i = 1; i <= edges.Extent(); i++) {
    const TopoDS_Edge& edge = TopoDS::Edge(edges(i));
    filletMaker.Add(radius, edge);
  }
  filletMaker.Build();
  return filletMaker.Shape();
}

TopoDS_Shape FilletAdaptiveRadius(
    const TopoDS_Shape& shape,
    const std::function<double(const TopoDS_Edge&)>& radiusByEdge) {
  BRepFilletAPI_MakeFillet filletMaker(shape);
  // Iterate edges
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(shape, TopAbs_EDGE, edges);

  for (size_t i = 1; i <= edges.Extent(); i++) {
    const TopoDS_Edge& edge = TopoDS::Edge(edges(i));
    double radius = radiusByEdge(edge);
    if (!std::isnan(radius)) {  // NaN => dont fillet this edge!
      filletMaker.Add(radius, edge);
    }
  }
  filletMaker.Build();
  return filletMaker.Shape();
}

}  // namespace OCCUtils::Fillet
