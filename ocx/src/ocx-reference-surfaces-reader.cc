//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-reference-surfaces-reader.h"

#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Compound.hxx>
#include <list>
#include <memory>
#include <utility>

#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-surface-reader.h"

OCXReferenceSurfacesReader::OCXReferenceSurfacesReader(
    std::shared_ptr<OCXContext> ctx)
    : ctx(std::move(ctx)) {}

TopoDS_Shape OCXReferenceSurfacesReader::ReadReferenceSurfaces(
    LDOM_Element &vesselN) {
  TopoDS_Compound referenceSurfacesAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(referenceSurfacesAssy);

  auto tag = std::string(vesselN.getTagName().GetString());
  std::size_t index = tag.find("Vessel");
  if (index == std::string::npos) {
    std::cout << "expected a Vessel element, got " << tag << std::endl;
    return referenceSurfacesAssy;
  }

  LDOM_Element refSrfsN =
      OCXHelper::GetFirstChild(vesselN, "ReferenceSurfaces");
  if (refSrfsN.isNull()) {
    std::cout << "could not find ReferenceSurfaces child node" << std::endl;
    return referenceSurfacesAssy;
  }

  std::list<TopoDS_Shape> shapes;
  // material design teal 50 400
  auto color =
      Quantity_Color(38 / 255.0, 16 / 255.0, 154 / 255.0, Quantity_TOC_RGB);

  auto *surfaceReader = new OCXSurfaceReader(ctx);

  LDOM_Node childN = refSrfsN.getFirstChild();

  while (childN != nullptr) {
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)childN;

      auto guid =
          std::string(surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString());
      auto name = std::string(surfaceN.getAttribute("name").GetString());

      std::cout << "read reference surface " << name << " guid=" << guid
                << ", type " << surfaceN.getTagName().GetString() << std::endl;

      auto referenceSurface = TopoDS_Shape();

      if ("SurfaceCollection" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("ReadExtrudedSurface" ==
                 OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        referenceSurface = surfaceReader->ReadSurface(surfaceN);
      } else {
        std::cerr << "found unknown referenceSurface type "
                  << surfaceN.getTagName().GetString() << " with guid '" << guid
                  << "'" << std::endl;
      }

      if (!referenceSurface.IsNull()) {
        ctx->RegisterSurface(guid, referenceSurface);

        if (OCXContext::CreateReferenceSurfaces) {
          TDF_Label surfL =
              ctx->OCAFShapeTool()->AddShape(referenceSurface, false);
          TDataStd_Name::Set(surfL, name.c_str());
          ctx->OCAFColorTool()->SetColor(surfL, color, XCAFDoc_ColorSurf);

          shapes.push_back(referenceSurface);
        }

      } else {
        std::cerr << "could not read surface with guid '" << guid << "'"
                  << std::endl;
      }
    }
    childN = childN.getNextSibling();
  }

  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(referenceSurfacesAssy, shape);
  }

  TDF_Label refSrfLabel =
      ctx->OCAFShapeTool()->AddShape(referenceSurfacesAssy, true);
  TDataStd_Name::Set(refSrfLabel, "Reference Surfaces");

  std::cout << "    registered #" << shapes.size()
            << " references surfaces found in "
            << refSrfsN.getTagName().GetString() << std::endl;

  return referenceSurfacesAssy;
}
