// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-reference-surfaces-reader.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-surface-reader.h"

namespace ocx {

TopoDS_Shape OCXReferenceSurfacesReader::ReadReferenceSurfaces(
    LDOM_Element const &vesselN) {
  TopoDS_Compound referenceSurfacesAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(referenceSurfacesAssy);

  LDOM_Element refSurfsN =
      OCXHelper::GetFirstChild(vesselN, "ReferenceSurfaces");
  if (refSurfsN.isNull()) {
    OCX_WARN("No ReferenceSurfaces child node found");
    return referenceSurfacesAssy;
  }

  std::list<TopoDS_Shape> shapes;
  // Material design teal 50 400
  auto color =
      Quantity_Color(38 / 255.0, 16 / 255.0, 154 / 255.0, Quantity_TOC_RGB);

  OCXSurfaceReader surfaceReader = OCXSurfaceReader(ctx);

  LDOM_Node childN = refSurfsN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      auto referenceSurface = TopoDS_Shape();

      LDOM_Element surfaceN = (LDOM_Element &)childN;
      char const *name = surfaceN.getAttribute("name").GetString();
      char const *guid = surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString();

      std::string referenceSurfaceType = OCXHelper::GetLocalTagName(surfaceN);

      OCX_INFO("Reading reference surface {} guid={} of type {}", name, guid,
               referenceSurfaceType);

      if (referenceSurfaceType == "SurfaceCollection") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Cone3D") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Cylinder3D") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "ExtrudedSurface") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "NURBSSurface") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Sphere3D") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else if (referenceSurfaceType == "Plane3D") {
        referenceSurface = surfaceReader.ReadSurface(surfaceN);
      } else {
        OCX_WARN(
            "Found unsupported reference surface type {} in surface {} guid={}",
            referenceSurfaceType, name, guid);
        childN = childN.getNextSibling();
        continue;
      }

      if (!referenceSurface.IsNull()) {
        ctx->RegisterSurface(referenceSurface, guid);

        if (OCXContext::CreateReferenceSurfaces) {
          TDF_Label surfL =
              ctx->OCAFShapeTool()->AddShape(referenceSurface, false);
          TDataStd_Name::Set(
              surfL, (std::string(name) + " " + std::string(guid)).c_str());
          ctx->OCAFColorTool()->SetColor(surfL, color, XCAFDoc_ColorSurf);

          shapes.push_back(referenceSurface);
        }
      } else {
        OCX_ERROR("Failed to read reference surface {} guid={} of type {}",
                  name, guid, referenceSurfaceType);
      }
    }
    childN = childN.getNextSibling();
  }

  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(referenceSurfacesAssy, shape);
  }

  TDF_Label refSurfLabel =
      ctx->OCAFShapeTool()->AddShape(referenceSurfacesAssy, true);
  TDataStd_Name::Set(refSurfLabel, "Reference Surfaces");

  OCX_INFO("Registered {} reference surfaces", shapes.size());

  return referenceSurfacesAssy;
}

}  // namespace ocx
