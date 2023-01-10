#include "occutils/occutils-boolean.h"

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

#include "occutils/occutils-list-utils.h"
#include "occutils/occutils-shape.h"

namespace OCCUtils::Boolean {

TopoDS_Shape Fuse(const TopTools_ListOfShape& arguments,
                  const TopTools_ListOfShape& tools) {
  if (arguments.Size() + tools.Size() == 1) {
    // Return that shape!
    if (arguments.Size() == 1) {
      return arguments.First();
    } else if (tools.Size() == 1) {
      return tools.First();
    } else {
      // Will never happen, just in case of hard issues to provide a hard return
      // path
      return {};
    }
  } else if (arguments.Size() + tools.Size() == 0) {
    // No shape => return no shape
    return {};
  } else if (arguments.Size() == 0) {
    throw std::invalid_argument("Fuse arguments must have at least one shape!");
  } else if (tools.Size() == 0) {
    throw std::invalid_argument("Fuse tools must have at least one shape!");
  }
  // Configure fuse
  BRepAlgoAPI_Fuse fuse;
  fuse.SetArguments(arguments);
  fuse.SetTools(tools);
  // Run fuse
  fuse.Build();
  return fuse.Shape();  // Raises NotDone if not done.
}

TopoDS_Shape Fuse(const TopTools_ListOfShape& shapes) {
  // We need "tools" and "arguments".
  // For fuse, the exact split does not matter,
  // but each must be size >= 1!
  auto [first, second] = ListUtils::SplitIntoHeadAndTail(shapes, 1);
  return Fuse(second, first);
}

TopoDS_Shape Fuse(const std::initializer_list<TopoDS_Shape>& shapes) {
  return Fuse(ListUtils::ToOCCList(shapes));
}

TopoDS_Shape Cut(const TopTools_ListOfShape& positive,
                 const TopTools_ListOfShape& negative) {
  if (positive.Size() == 0) {
    throw std::invalid_argument("Cut positive must have at least one shape!");
  }
  if (negative.Size() == 0) {
    // Just fuse positive
    return Fuse(positive);
  }
  // Configure fuse
  BRepAlgoAPI_Cut cut;
  cut.SetArguments(positive);
  cut.SetTools(negative);
  // Run cut
  cut.Build();
  return cut.Shape();  // Raises NotDone if not done.
}

TopoDS_Shape Cut(const TopoDS_Shape& positive, const TopoDS_Shape& negative) {
  return Cut(ListUtils::ToOCCList({positive}),
             ListUtils::ToOCCList({negative}));
}

TopoDS_Shape Cut(const TopoDS_Shape& positive,
                 const TopTools_ListOfShape& negative) {
  return Cut(ListUtils::ToOCCList({positive}), negative);
}

TopoDS_Shape Cut(const TopoDS_Shape& positive,
                 const std::initializer_list<TopoDS_Shape>& negative) {
  return Cut(ListUtils::ToOCCList({positive}), ListUtils::ToOCCList(negative));
}

TopoDS_Shape Cut(const std::vector<TopoDS_Solid>& positive,
                 const std::vector<TopoDS_Solid>& negative) {
  return Cut(Shapes::FromSolids(positive), Shapes::FromSolids(negative));
}

TopoDS_Shape Cut(const TopoDS_Solid& positive,
                 const std::vector<TopoDS_Solid>& negative) {
  return Cut({positive}, Shapes::FromSolids(negative));
}

TopoDS_Shape Cut(const std::vector<TopoDS_Face>& positive,
                 const std::vector<TopoDS_Face>& negative) {
  return Cut(Shapes::FromFaces(positive), Shapes::FromFaces(negative));
}

TopoDS_Shape Cut(const TopoDS_Face& positive,
                 const std::vector<TopoDS_Face>& negative) {
  return Cut({positive}, Shapes::FromFaces(negative));
}

TopoDS_Shape Fuse(const std::vector<TopoDS_Solid>& shapes) {
  return Fuse(Shapes::FromSolids(shapes));
}

TopoDS_Shape Fuse(const std::initializer_list<TopoDS_Solid>& shapes) {
  return Fuse(Shapes::FromSolids(shapes));
}

TopoDS_Shape Fuse(const std::vector<TopoDS_Face>& shapes) {
  return Fuse(Shapes::FromFaces(shapes));
}

TopoDS_Shape Fuse(const std::initializer_list<TopoDS_Face>& shapes) {
  return Fuse(Shapes::FromFaces(shapes));
}

}  // namespace OCCUtils::Boolean
