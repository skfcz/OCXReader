/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
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

#include "ocx/internal/ocx-reference-surfaces.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-surface-reader.h"

namespace ocx::vessel::reference_surfaces {

void ReadReferenceSurfaces(LDOM_Element const &vesselN) {
  LDOM_Element refSurfsN =
      ocx::helper::GetFirstChild(vesselN, "ReferenceSurfaces");
  if (refSurfsN.isNull()) {
    OCX_WARN("No ReferenceSurfaces child node found");
    return;
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = refSurfsN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto referenceSurface = TopoDS_Shape();

      LDOM_Element surfaceN = (LDOM_Element &)childN;
      std::unique_ptr<ocx::helper::OCXMeta> meta =
          ocx::helper::GetOCXMeta(surfaceN);

      std::string referenceSurfaceType = ocx::helper::GetLocalTagName(surfaceN);
      if (referenceSurfaceType == "SurfaceCollection") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Cone3D") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Cylinder3D") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "ExtrudedSurface") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "NURBSSurface") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Sphere3D") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Plane3D") {
        referenceSurface = ocx::surface::ReadSurface(surfaceN);
      } else {
        OCX_WARN(
            "Found unsupported reference surface type {} in surface {} guid={}",
            referenceSurfaceType, meta->name, meta->guid);
        childN = childN.getNextSibling();
        continue;
      }

      if (!referenceSurface.IsNull()) {
        OCXContext::GetInstance()->RegisterShape(
            surfaceN, ExtendedShape(referenceSurface, "ReferenceSurface"));

        if (OCXContext::CreateReferenceSurfaces) {
          // Material design teal 50 400
          auto color = Quantity_Color(38 / 255.0, 16 / 255.0, 154 / 255.0,
                                      Quantity_TOC_RGB);
          TDF_Label surfL =
              OCXContext::GetInstance()->OCAFShapeTool()->AddShape(
                  referenceSurface, false);
          TDataStd_Name::Set(
              surfL, (std::string(meta->name) + " " + std::string(meta->guid))
                         .c_str());
          OCXContext::GetInstance()->OCAFColorTool()->SetColor(
              surfL, color, XCAFDoc_ColorSurf);

          shapes.push_back(referenceSurface);
        }
      } else {
        OCX_ERROR("Failed to read reference surface {} guid={} of type {}",
                  meta->name, meta->guid, referenceSurfaceType);
      }
    }
    childN = childN.getNextSibling();
  }

  TopoDS_Compound referenceSurfacesAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(referenceSurfacesAssy);

  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(referenceSurfacesAssy, shape);
  }

  TDF_Label refSurfLabel = OCXContext::GetInstance()->OCAFShapeTool()->AddShape(
      referenceSurfacesAssy, true);
  TDataStd_Name::Set(refSurfLabel, "Reference Surfaces");

  OCX_INFO("Registered {} reference surfaces", shapes.size());
}

}  // namespace ocx::vessel::reference_surfaces
