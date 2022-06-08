//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include "OCXPanelReader.h"
#include "OCXHelper.h"
#include "OCXCurveReader.h"
#include "OCXSurfaceReader.h"


OCXPanelReader::OCXPanelReader(OCXContext *ctx) {
    this->ctx = ctx;
}

TopoDS_Shape OCXPanelReader::ParsePanels(LDOM_Element &vesselN, TDF_Label vesselL) {
    TopoDS_Compound compound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (compound);

    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = vesselN.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aNextElement = (LDOM_Element &) aChildNode;
            if ("Panel" == OCXHelper::GetLocalTagName(aNextElement)) {
                TopoDS_Shape shape = ParsePanel(aNextElement, vesselL);
                if ( !shape.IsNull()) {
                    aBuilder.Add(compound, shape);
                }


            }
        }
        aChildNode = aChildNode.getNextSibling();
    }

    return compound;
}

TopoDS_Shape OCXPanelReader::ParsePanel(LDOM_Element &panelN, TDF_Label vesselL) {

    std::string id = std::string(panelN.getAttribute("id").GetString());

    std::cout << "parse Panel " << id << ", name='" << panelN.getAttribute("name").GetString() << "'"<<  std::endl;

    TopoDS_Compound compound;
    TopoDS_Wire outerContur;

    BRep_Builder compoundBuilder;
    compoundBuilder.MakeCompound (compound);


    LDOM_Element outerContourN = OCXHelper::GetFirstChild(panelN, "OuterContour");
    if (outerContourN.isNull()) {
        std::cerr << "could not find OuterContour for Panel " << id << std::endl;
        return TopoDS_Shape();
    }
    try {
        OCXCurveReader * reader = new OCXCurveReader( ctx);
        TopoDS_Shape curveShape = reader->ReadCurve(outerContourN);
        if ( curveShape.ShapeType() != TopAbs_WIRE) {
            std::cerr << "expect a closed shape as OuterContur, but got " << curveShape.ShapeType() << " in "
                      << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        }
        outerContur = TopoDS::Wire(curveShape);


        if ( OCXContext::CreatePanelContours) {
            compoundBuilder.Add(compound, outerContur);
        }

    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring OuterContur from panel "
                  << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        return compound;
    }

    if ( ! OCXContext::CreatePanelSurfaces) {
        return compound;
    }

    LDOM_Element unboundedGeometryN = OCXHelper::GetFirstChild(panelN, "UnboundedGeometry");
    if (unboundedGeometryN.isNull()) {
        std::cerr << "could not find UnboundedGeometry for Panel " << id << std::endl;
        return compound;
    }

    // The unbounded geometry is either a surface, or a surface reference, or a grid reference

    TopoDS_Face surface = TopoDS_Face();

    LDOM_Element gridRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");
    LDOM_Element surfaceRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");

    if ( !gridRefN.isNull()) {
        std::string guid = std::string(gridRefN.getAttribute( ctx->OCXGUIDRef()).GetString());

        std::cout << "use GridRef " << guid << " as reference surface" << std::endl;
        surface = ctx->LookupSurface(guid);
        if ( surface.IsNull()) {
            std::cerr << "failed to lookup grid reference to " << guid << " contained in panel " << id << std::endl;
            return compound;
        }
    } else if ( ! surfaceRefN.isNull()) {
        std::string guid = std::string(surfaceRefN.getAttribute( ctx->OCXGUIDRef()).GetString());
        std::cout << "use SurfaceRef " << guid << " as reference surface" << std::endl;
        surface = ctx->LookupSurface(guid);
        if ( surface.IsNull()) {
            std::cerr << "failed to lookup surface reference to " << guid << " contained in panel " << id << std::endl;
            return compound;
        }
    } else {




        LDOM_Node aChildNode = unboundedGeometryN.getFirstChild();
        while (aChildNode != NULL) {
            const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
            if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
                break;
            if (aNodeType == LDOM_Node::ELEMENT_NODE) {
                LDOM_Element surfaceN = (LDOM_Element &) aChildNode;
                std::string tag = OCXHelper::GetLocalTagName(surfaceN);
                if ("Cone3D" == tag || "Cylinder3D" == tag || "ExtrudedSurface" == tag ||
                "NURBSSurface" == tag || "Plane3D" == tag || "Sphere3D" == tag) {

                    OCXSurfaceReader *surfaceReader = new OCXSurfaceReader(ctx);
                    surface = surfaceReader->ReadSurface(surfaceN);
                    if (surface.IsNull()) {
                        std::cerr
                                << "failed to read surface definition from Panel/UnboundedGeometry contained in panel "
                                << id << std::endl;
                        return compound;
                    }
                    compoundBuilder.Add(surface, outerContur);
                    break;
                } else {
                    std::cerr << "found unsupported children element " << surfaceN.getTagName().GetString()
                              << " in Panel/UnboundedGeometry in panel"
                              << id << std::endl;
                }
            }
            aChildNode = aChildNode.getNextSibling();
        }
    } // end reading surface

    return compound;

}

