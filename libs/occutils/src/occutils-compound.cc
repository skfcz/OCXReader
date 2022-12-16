#include "occutils/occutils-compound.h"

#include <BRep_Builder.hxx>

#include "occutils/occutils-list-utils.h"
#include "occutils/occutils-shape.h"

namespace OCCUtils::Compound {

template <typename T>
TopoDS_Compound ToCompound(const T& shapes) {
  BRep_Builder builder;
  TopoDS_Compound compound;
  builder.MakeCompound(compound);
  for (const auto& shape : shapes) {
    if (!shape.IsNull()) {
      builder.Add(compound, shape);
    }
  }
  return compound;
}

TopoDS_Compound From(const TopTools_ListOfShape& shapes) {
  return ToCompound(shapes);
}

TopoDS_Compound From(const std::vector<TopoDS_Shape>& shapes) {
  return ToCompound(shapes);
}

TopoDS_Compound From(const std::vector<TopoDS_Face>& shapes) {
  return ToCompound(shapes);
}

TopoDS_Compound From(const std::vector<TopoDS_Solid>& shapes) {
  return ToCompound(shapes);
}

}  // namespace OCCUtils::Compound
