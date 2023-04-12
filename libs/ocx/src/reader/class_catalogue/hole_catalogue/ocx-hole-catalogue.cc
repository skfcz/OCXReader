/***************************************************************************
 *   Created on: 28 Nov 2022                                               *
 ***************************************************************************
 *   Copyright (c) 2022, Carsten Zerbst (carsten.zerbst@groy-groy.de)      *
 *   Copyright (c) 2022, Paul Buechner                                     *
 *                                                                         *
 *   This file is part of the OCXReader library.                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   version 2.1 as published by the Free Software Foundation.             *
 *                                                                         *
 ***************************************************************************/

#include "ocx/internal/ocx-hole-catalogue.h"

#include <LDOM_Element.hxx>
#include <TopoDS_Shape.hxx>

#include "ocx/ocx-helper.h"

namespace ocx::hole_catalogue {

void ReadHoleCatalogue(LDOM_Element const &catalogueN) {
  LDOM_Element holeCatalogueN =
      ocx::helper::GetFirstChild(catalogueN, "HoleShapeCatalogue");
  if (holeCatalogueN.isNull()) {
    OCX_WARN("No HoleShapeCatalogue child node found.")
    return;
  }

  // Read Hole
  LDOM_Element hole2DEle = ocx::helper::GetFirstChild(holeCatalogueN, "Hole2D");
  while (!hole2DEle.isNull()) {
    LDOM_Element contourEle = ocx::helper::GetFirstChild(hole2DEle, "Contour");
    if (!contourEle.isNull()) {
      // TODO: HoleShapes from parametric types ( Rectangular etc.)
      // LDOM_Node curveEle = contourEle.getFirstChild();

      TopoDS_Shape hole2dShape =
          ocx::reader::shared::curve::ReadCurve(contourEle);

      if (!hole2dShape.IsNull()) {
        auto holeMeta = ocx::helper::GetOCXMeta(hole2DEle);
        // Add to hole catalogue
        OCXContext::GetInstance()->RegisterHoleShape(holeMeta->guid,
                                                     hole2dShape);
      }
    }

    hole2DEle = hole2DEle.GetSiblingByTagName();
  }
}
}  // namespace ocx::hole_catalogue
