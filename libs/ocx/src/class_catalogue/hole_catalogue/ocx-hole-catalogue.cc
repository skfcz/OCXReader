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

#include <LDOM_Element.hxx>
#include <memory>

#include "ocx/internal/ocx-utils.h"
#include "ocx/internal/ocx-hole-catalogue.h"
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
   LDOM_Element hole2Dele =
       ocx::helper::GetFirstChild(holeCatalogueN, "Hole2D");
   while (!hole2Dele.isNull()) {
     LDOM_Element contourEle =
         ocx::helper::GetFirstChild(hole2Dele, "Contour");
     if (!contourEle.isNull()) {
       TopoDS_Shape hole2dShape = TopoDS_Shape();
       LDOM_Node curveEle = contourEle.getFirstChild();
       //if (curveEle.getNodeName().GetString() ==
       //    std::string("ocx:CompositeCurve3D"))
         hole2dShape = ocx::shared::curve::ReadCurve(
             (LDOM_Element &)contourEle);

       if (!hole2dShape.IsNull()) {
         std::string guid =
             hole2Dele.getAttribute("ocx:GUIDRef").GetString();
         // Add to hole catalogue
         OCXContext::GetInstance()->RegisterHoleShape(guid, hole2dShape);
       }
     }

     hole2Dele = hole2Dele.GetSiblingByTagName();
   }
 

}
}  // namespace ocx::hole_catalogue