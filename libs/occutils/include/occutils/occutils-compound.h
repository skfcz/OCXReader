#pragma once

#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <initializer_list>
#include <vector>

#include "occutils/occutils-list-utils.h"

namespace OCCUtils::Compound {

/**
 * Create a compound from multiple shapes
 */
TopoDS_Compound From(const TopTools_ListOfShape& shapes);

/**
 * Create a compound from multiple shapes
 */
TopoDS_Compound From(const std::vector<TopoDS_Shape>& shapes);
TopoDS_Compound From(const std::vector<TopoDS_Face>& shapes);
TopoDS_Compound From(const std::vector<TopoDS_Solid>& shapes);

}  // namespace OCCUtils::Compound
