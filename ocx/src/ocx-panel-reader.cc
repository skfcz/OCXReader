// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#include "ocx/internal/ocx-panel-reader.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "ocx/internal/ocx-curve-reader.h"
#include "ocx/internal/ocx-helper.h"
#include "ocx/internal/ocx-log.h"
#include "ocx/internal/ocx-surface-reader.h"

namespace ocx {

TopoDS_Shape OCXPanelReader::ReadPanels(LDOM_Element const &vesselN) const {
  OCX_INFO("Start reading panels...");

  std::list<TopoDS_Shape> shapes;

  LDOM_Node aChildNode = vesselN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aElement = (LDOM_Element &)aChildNode;

      if (OCXHelper::GetLocalTagName(aElement) == "Panel") {
        char const *id = aElement.getAttribute("id").GetString();
        char const *guid = aElement.getAttribute(ctx->OCXGUIDRef()).GetString();

        TopoDS_Shape panels = ReadPanel(aElement, id, guid);
        if (!panels.IsNull()) {
          shapes.push_back(panels);
        }
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  if (shapes.empty()) {
    OCX_WARN("No panels found.");
    return {};
  }

  TopoDS_Compound panelsAssy;
  BRep_Builder panelsBuilder;
  panelsBuilder.MakeCompound(panelsAssy);

  for (const TopoDS_Shape &shape : shapes) {
    panelsBuilder.Add(panelsAssy, shape);
  }

  TDF_Label panelsL = ctx->OCAFShapeTool()->AddShape(panelsAssy, true);
  TDataStd_Name::Set(panelsL, "Panels");

  OCX_INFO("Finished reading panels...");

  return panelsAssy;
}

TopoDS_Shape OCXPanelReader::ReadPanel(LDOM_Element const &panelN,
                                       std::string_view id,
                                       std::string_view guid) const {
  char const *name = panelN.getAttribute("name").GetString();

  OCX_DEBUG("Reading panel {} id={} guid={}", name, id, guid);

  // Snapshot of Panel and Plate Create-Options
  bool CreatePanelContours = OCXContext::CreatePanelContours;
  bool CreatePanelSurfaces = OCXContext::CreatePanelSurfaces;
  bool CreatePlateSurfaces = OCXContext::CreatePlateSurfaces;

  std::list<TopoDS_Shape> shapes;

  // Read the Contour
  auto outerContour = TopoDS_Wire();  // retain scope for CreatePanelSurfaces
  if (CreatePanelContours) {
    outerContour = ReadPanelOuterContour(panelN, id, guid);
    if (!outerContour.IsNull()) {
      // Material Design ...
      auto contourColor =
          Quantity_Color(20 / 256.0, 20 / 256.0, 20.0 / 256, Quantity_TOC_RGB);

      // Add Contour node in OCAF
      TDF_Label contourLabel =
          ctx->OCAFShapeTool()->AddShape(outerContour, false);
      TDataStd_Name::Set(contourLabel,
                         ("Contour (" + std::string(id) + ")").c_str());
      ctx->OCAFColorTool()->SetColor(contourLabel, contourColor,
                                     XCAFDoc_ColorCurv);
      shapes.push_back(outerContour);
    }
  } else {
    OCX_ERROR(
        "Failed to read OuterContour in ReadPanel with panel {} id={} "
        "guid={}",
        name, id, guid);

    // Disable PanelSurfaces and PlateSurfaces if they are enabled
    if (OCXContext::CreatePanelSurfaces) {
      OCX_WARN(
          "PanelSurfaces creation is enabled but but PanelContours creation "
          "failed. Disabling PanelSurfaces.");
      CreatePanelSurfaces = false;
    }
    if (OCXContext::CreatePlateSurfaces) {
      OCX_WARN(
          "PlateSurfaces creation is enabled, but PanelContours creation "
          "failed. Disabling PlateSurfaces creation.");
      CreatePlateSurfaces = false;
    }
  }

  // Read the PanelSurface
  auto panelSurface = TopoDS_Shape();  // retain scope for CreatePlateSurfaces
  if (CreatePanelSurfaces) {
    panelSurface = ReadPanelSurface(panelN, outerContour, id, guid);
    if (!panelSurface.IsNull()) {
      // Material Design Light Green 50 300
      auto surfaceColor = Quantity_Color(174 / 256.0, 213 / 256.0, 129.0 / 256,
                                         Quantity_TOC_RGB);

      // Add PanelSurface node in OCAF
      TDF_Label panelSurfaceLabel =
          ctx->OCAFShapeTool()->AddShape(panelSurface, false);
      TDataStd_Name::Set(panelSurfaceLabel,
                         ("Surface (" + std::string(id) + ")").c_str());
      ctx->OCAFColorTool()->SetColor(panelSurfaceLabel, surfaceColor,
                                     XCAFDoc_ColorSurf);
      shapes.push_back(panelSurface);
    } else {
      OCX_ERROR(
          "Failed to read PanelSurface in ReadPanel with panel {} id={} "
          "guid={}",
          name, id, guid);

      // Disable PlateSurfaces creation if it's enabled
      if (OCXContext::CreatePlateSurfaces) {
        OCX_WARN(
            "PlateSurfaces creation is enabled, but PanelSurface creation "
            "failed. Disabling PlateSurfaces creation.");
        CreatePlateSurfaces = false;
      }
    }
  }

  // Read the PlateSurfaces (depends on PanelSurface)
  if (CreatePanelSurfaces && CreatePlateSurfaces) {
    TopoDS_Shape plates = ReadPlates(panelN, panelSurface, id, guid);
    if (!plates.IsNull()) {
      // Add Plates node in OCAF
      TDF_Label platesLabel = ctx->OCAFShapeTool()->AddShape(plates, false);
      TDataStd_Name::Set(platesLabel,
                         ("Plates (" + std::string(id) + ")").c_str());
      shapes.push_back(plates);
    } else {
      OCX_ERROR(
          "Failed to read Plates in ReadPanel with panel {} id={} guid={}",
          name, id, guid);
    }
  } else if (!OCXContext::CreatePanelSurfaces &&
             OCXContext::CreatePlateSurfaces) {
    OCX_WARN(
        "PlateSurfaces creation is enabled, but PanelSurface creation is "
        "disabled. Skipping PlateSurfaces creation.");
  }

  // Read the stiffeners
  if (OCXContext::CreateStiffenerTraces) {
    TopoDS_Shape stiffeners = ReadStiffeners(panelN, id, guid);
    if (!stiffeners.IsNull()) {
      TDF_Label label = ctx->OCAFShapeTool()->AddShape(stiffeners, true);
      TDataStd_Name::Set(label, "Stiffeners");
      shapes.push_back(stiffeners);
    }
  }

  TopoDS_Compound panelAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(panelAssy);
  for (const TopoDS_Shape &shape : shapes) {
    compoundBuilder.Add(panelAssy, shape);
  }

  TDF_Label panelLabel = ctx->OCAFShapeTool()->AddShape(panelAssy, true);
  TDataStd_Name::Set(
      panelLabel, (std::string(name) + " (" + std::string(id) + ")").c_str());

  return panelAssy;
}

TopoDS_Wire OCXPanelReader::ReadPanelOuterContour(LDOM_Element const &panelN,
                                                  std::string_view id,
                                                  std::string_view guid) const {
  auto outerContour = TopoDS_Wire();

  LDOM_Element outerContourN = OCXHelper::GetFirstChild(panelN, "OuterContour");
  if (outerContourN.isNull()) {
    OCX_ERROR(
        "No OuterContour child node found in ReadPanelOuterContour with panel "
        "id={} guid={}",
        id, guid);
    return outerContour;
  }

  try {
    OCXCurveReader curveReader(ctx);
    TopoDS_Shape curveShape = curveReader.ReadCurve(outerContourN);
    if (curveShape.ShapeType() != TopAbs_WIRE) {
      OCX_ERROR(
          "OuterContour child node in ReadPanelOuterContour with panel id={} "
          "guid={} is not a wire, expect a closed shape as OuterContour, but "
          "got {}",
          id, guid, curveShape.ShapeType());
    }
    return TopoDS::Wire(curveShape);
  } catch (Standard_Failure const &exp) {
    OCX_ERROR(
        "Failed to read OuterContour child node in ReadPanelOuterContour with "
        "panel id={} guid={}, exiting with exception: {}",
        id, guid, exp.GetMessageString());
    return outerContour;
  }
}

TopoDS_Shape OCXPanelReader::ReadPanelSurface(LDOM_Element const &panelN,
                                              TopoDS_Wire const &outerContour,
                                              std::string_view id,
                                              std::string_view guid) const {
  LDOM_Element unboundedGeometryN =
      OCXHelper::GetFirstChild(panelN, "UnboundedGeometry");
  if (unboundedGeometryN.isNull()) {
    OCX_ERROR(
        "No UnboundedGeometry child node found in ReadPanelSurface with panel "
        "id={} guid={}",
        id, guid);
    return {};
  }

  // Unbounded geometry is either a shell, a shell reference, or a grid
  // reference
  LDOM_Element refN{};
  if (LDOM_Element gridRefN =
          OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");
      !gridRefN.isNull()) {
    OCX_DEBUG(
        "Using GridRef in ReadPanelSurface as ReferenceSurface with panel "
        "id={} guid={}",
        id, guid);
    refN = gridRefN;
  } else if (LDOM_Element surfaceRefN =
                 OCXHelper::GetFirstChild(unboundedGeometryN, "SurfaceRef");
             !surfaceRefN.isNull()) {
    OCX_DEBUG(
        "Using SurfaceRef in ReadPanelSurface as ReferenceSurface with "
        "panel id={} guid={}",
        id, guid);
    refN = surfaceRefN;
  } else {
    OCX_DEBUG(
        "No GridRef or SurfaceRef child node found in "
        "ReadPanelSurface/UnboundedGeometry with panel id={} guid={}. Try "
        "reading directly from UnboundedGeometry.",
        id, guid);
  }

  // Read from GridRef or SurfaceRef
  if (!refN.isNull()) {
    char const *refGuid = refN.getAttribute(ctx->OCXGUIDRef()).GetString();

    TopoDS_Shape surface = ctx->LookupSurface(refGuid);
    if (surface.IsNull()) {
      OCX_ERROR(
          "Failed to lookup ReferenceSurface guid={} in ReadPanelSurface "
          "with panel id={} guid={}",
          refGuid, id, guid);
      return {};
    }

    // TODO: Both GridRef and SurfaceRef allow for specifying an offset
    // parameter of the referenced Surface. This is currently ignored.

    if (surface.ShapeType() == TopAbs_FACE) {
      auto facebuilder =
          BRepBuilderAPI_MakeFace(TopoDS::Face(surface), outerContour);
      facebuilder.Build();
      if (!facebuilder.IsDone()) {
        OCX_ERROR(
            "Failed to create restricted PanelSurface from ReferenceSurface "
            "guid={} and given OuterContour in ReadPanelSurface with panel "
            "id={} guid={}",
            refGuid, id, guid);
        return {};
      }

      return facebuilder.Face();
    }

    // TODO: Implement TopAbs_SHELL surface type
    OCX_ERROR(
        "ReferenceSurface guid={} is not a face, but a {} in "
        "ReadPanelSurface with panel id={} guid={}. Currently only faces are "
        "supported as ReferenceSurface.",
        refGuid, surface.ShapeType(), id, guid);

    // TODO: limit the faces in the shell by the outer contour
    // Also kind of a duplicate of ReadPlate

    return {};
  }

  // Read directly from UnboundedGeometry
  LDOM_Node aChildNode = unboundedGeometryN.getFirstChild();
  while (aChildNode != nullptr) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element surfaceN = (LDOM_Element &)aChildNode;

      OCXSurfaceReader surfaceReader(ctx);
      if (TopoDS_Shape surface = surfaceReader.ReadSurface(surfaceN);
          !surface.IsNull()) {
        return surface;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }

  OCX_ERROR(
      "Failed to read surface from UnboundedGeometry in "
      "ReadPanelSurface with panel id={} guid={}",
      id, guid);

  return {};
}

TopoDS_Shape OCXPanelReader::ReadPlates(LDOM_Element const &panelN,
                                        TopoDS_Shape const &referenceSurface,
                                        std::string_view id,
                                        std::string_view guid) const {
  LDOM_Element composedOfN = OCXHelper::GetFirstChild(panelN, "ComposedOf");
  if (composedOfN.isNull()) {
    OCX_ERROR(
        "No ComposedOf child node found in ReadPlates with panel id={} guid={}",
        id, guid);
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = composedOfN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element plateN = (LDOM_Element &)childN;

      if (OCXHelper::GetLocalTagName(plateN) == "Plate") {
        char const *pId = plateN.getAttribute("id").GetString();
        char const *pGuid = plateN.getAttribute(ctx->OCXGUIDRef()).GetString();

        if (TopoDS_Shape plate =
                ReadPlate(plateN, referenceSurface, pId, pGuid);
            !plate.IsNull()) {
          shapes.push_back(plate);
        }
      }
    }
    childN = childN.getNextSibling();
  }

  TopoDS_Compound plateAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(plateAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(plateAssy, shape);
  }

  TDF_Label plateLabel = ctx->OCAFShapeTool()->AddShape(plateAssy, true);
  TDataStd_Name::Set(plateLabel, "Plate");

  return plateAssy;
}

TopoDS_Shape OCXPanelReader::ReadPlate(LDOM_Element const &plateN,
                                       TopoDS_Shape const &referenceSurface,
                                       std::string_view id,
                                       std::string_view guid) const {
  OCX_DEBUG("Reading Plate id={} guid={}", id, guid);

  LDOM_Element outerContourN = OCXHelper::GetFirstChild(plateN, "OuterContour");
  if (outerContourN.isNull()) {
    OCX_ERROR(
        "No OuterContour child node found in ReadPlate with plate id={} "
        "guid={}",
        id, guid);
    return {};
  }

  // Material Design green 50
  auto plateColor =
      Quantity_Color(76 / 255.0, 175 / 255.0, 80 / 255.0, Quantity_TOC_RGB);

  OCXCurveReader curveReader(ctx);
  TopoDS_Shape curveShape = curveReader.ReadCurve(outerContourN);
  if (curveShape.ShapeType() != TopAbs_WIRE) {
    OCX_ERROR(
        "OuterContour child node in ReadPlate with plate id={} guid={} is "
        "not a wire, expect a closed shape as OuterContour, but got {}",
        id, guid, curveShape.ShapeType());
    return {};
  }
  TopoDS_Wire outerContour = TopoDS::Wire(curveShape);

  if (referenceSurface.ShapeType() == TopAbs_FACE) {
    TopoDS_Face face = TopoDS::Face(referenceSurface);
    auto faceBuilder = BRepBuilderAPI_MakeFace(face, outerContour);
    faceBuilder.Build();
    if (!faceBuilder.IsDone()) {
      OCX_ERROR(
          "Failed to create restricted PlateSurface from ReferenceSurface "
          "and given OuterContour in ReadPlate with plate id={} guid={}",
          id, guid);
      return {};
    }

    TDF_Label label = ctx->OCAFShapeTool()->AddShape(faceBuilder.Face(), true);
    TDataStd_Name::Set(
        label,
        ("Plate " + std::string(id) + " (" + std::string(guid) + ")").c_str());
    ctx->OCAFColorTool()->SetColor(label, plateColor, XCAFDoc_ColorSurf);

    return faceBuilder.Face();
  }

  // TODO: Implement TopAbs_SHELL surface type
  OCX_ERROR(
      "ReferenceSurface is not a face, but a {} in "
      "ReadPlate with plate id={} guid={}. Currently only faces are "
      "supported as ReferenceSurface.",
      referenceSurface.ShapeType(), id, guid);

  // TODO: limit the faces in the shell by the outer contour

  return {};
}

TopoDS_Shape OCXPanelReader::ReadStiffeners(LDOM_Element const &panelN,
                                            std::string_view id,
                                            std::string_view guid) const {
  LDOM_Element composedOfN = OCXHelper::GetFirstChild(panelN, "StiffenedBy");
  if (composedOfN.isNull()) {
    OCX_ERROR(
        "No StiffenedBy child node found in ReadStiffeners with panel id={} "
        "guid={}",
        id, guid);
    return {};
  }

  std::list<TopoDS_Shape> shapes;

  LDOM_Node childN = composedOfN.getFirstChild();
  while (childN != nullptr) {
    const LDOM_Node::NodeType aNodeType = childN.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE) break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element stiffenerN = (LDOM_Element &)childN;

      if (OCXHelper::GetLocalTagName(stiffenerN) == "Stiffener") {
        char const *sId = stiffenerN.getAttribute("id").GetString();
        char const *sGuid =
            stiffenerN.getAttribute(ctx->OCXGUIDRef()).GetString();

        if (TopoDS_Shape stiffener = ReadStiffener(stiffenerN, sId, sGuid);
            !stiffener.IsNull()) {
          shapes.push_back(stiffener);
        }
      }
    }
    childN = childN.getNextSibling();
  }

  if (shapes.empty()) {
    OCX_WARN(
        "No Stiffener child nodes found in ReadStiffeners with panel id={} "
        "guid={}",
        id, guid);
    return {};
  }

  TopoDS_Compound stiffenersAssy;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(stiffenersAssy);
  for (TopoDS_Shape const &shape : shapes) {
    compoundBuilder.Add(stiffenersAssy, shape);
  }

  return stiffenersAssy;
}

TopoDS_Shape OCXPanelReader::ReadStiffener(LDOM_Element const &stiffenerN,
                                           std::string_view id,
                                           std::string_view guid) const {
  OCX_DEBUG("Reading Stiffener id={} guid={}", id, guid);

  LDOM_Element traceN = OCXHelper::GetFirstChild(stiffenerN, "TraceLine");
  // Also check for <ocx:TraceLine /> node
  if (traceN.isNull() || traceN.getFirstChild().isNull()) {
    OCX_ERROR(
        "No TraceLine child node found in ReadStiffener with stiffener "
        "id={} guid={}",
        id, guid);
    return {};
  }

  OCXCurveReader curveReader(ctx);
  TopoDS_Shape trace = curveReader.ReadCurve(traceN);
  if (trace.IsNull()) {
    OCX_ERROR(
        "Failed to read TraceLine in ReadStiffener with stiffener id={} "
        "guid={}",
        id, guid);
    return {};
  }

  // material design red 50 500
  auto plateColor =
      Quantity_Color(244 / 255.0, 67 / 255.0, 54 / 255.0, Quantity_TOC_RGB);

  TDF_Label label = ctx->OCAFShapeTool()->AddShape(trace, false);
  TDataStd_Name::Set(
      label, ("Stiffener " + std::string(id) + " (" + std::string(guid) + ")")
                 .c_str());
  ctx->OCAFColorTool()->SetColor(label, plateColor, XCAFDoc_ColorSurf);

  return trace;

  // TODO: Add simplified geometry representation of stiffener
}

TopoDS_Shape OCXPanelReader::ReadBrackets(LDOM_Element &panelN) {
  return TopoDS_Shape();
}

TopoDS_Shape OCXPanelReader::ReadBracket(LDOM_Element &bracketN) {
  return TopoDS_Shape();
}

}  // namespace ocx
