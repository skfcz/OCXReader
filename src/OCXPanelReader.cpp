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
#include <list>
#include <TDataStd_Name.hxx>
#include <Quantity_Color.hxx>
#include "OCXPanelReader.h"
#include "OCXHelper.h"
#include "OCXCurveReader.h"
#include "OCXSurfaceReader.h"


OCXPanelReader::OCXPanelReader(OCXContext *ctx) {
    this->ctx = ctx;
}

TopoDS_Shape OCXPanelReader::ParsePanels(LDOM_Element &vesselN) {

    std::list<TopoDS_Shape> shapes;


    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = vesselN.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aNextElement = (LDOM_Element &) aChildNode;
            if ("Panel" == OCXHelper::GetLocalTagName(aNextElement)) {
                TopoDS_Shape shape = ParsePanel(aNextElement);
                if ( !shape.IsNull()) {
                    shapes.push_back( shape);
                }
            }
        }
        aChildNode = aChildNode.getNextSibling();
    }

    TopoDS_Compound panelsAssy;
    BRep_Builder panelsBuilder;
    panelsBuilder.MakeCompound (panelsAssy);

    for( TopoDS_Shape shape : shapes) {
        panelsBuilder.Add(panelsAssy, shape);
    }

    return panelsAssy;
}

TopoDS_Shape OCXPanelReader::ParsePanel(LDOM_Element &panelN) {

    std::string id = std::string(panelN.getAttribute("id").GetString());
    std::string name = std::string( panelN.getAttribute("name").GetString());

    std::cout << "parse Panel " << id << ", name='" << name << "'"<<  std::endl;

    std::list<TopoDS_Shape> shapes;

    TopoDS_Wire outerContur;

    Quantity_Color contourColor = Quantity_Color(20/256.0, 20/256.0, 20/256, Quantity_TOC_RGB);

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
            TDF_Label surfL = ctx->GetOCAFShapeTool()->AddShape( outerContur, false);
            TDataStd_Name::Set( surfL, (id + " Contour").c_str());
            ctx->GetOCAFColorTool()->SetColor( surfL, contourColor, XCAFDoc_ColorSurf );

            shapes.push_back( outerContur);

        }

    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring OuterContur from panel "
                  << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        return TopoDS_Shape();
    }

    // TODO: create surface
    /*if (  OCXContext::CreatePanelSurfaces) {
        return panelAssy;


    LDOM_Element unboundedGeometryN = OCXHelper::GetFirstChild(panelN, "UnboundedGeometry");
    if (unboundedGeometryN.isNull()) {
        std::cerr << "could not find UnboundedGeometry for Panel " << id << std::endl;
        return panelAssy;
    }

    // The unbounded geometry is either a surface, or a surface reference, or a grid reference

    TopoDS_Shape surface = TopoDS_Shape();

    LDOM_Element gridRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");
    LDOM_Element surfaceRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");

    if ( !gridRefN.isNull()) {
        std::string guid = std::string(gridRefN.getAttribute( ctx->OCXGUIDRef()).GetString());

        std::cout << "use GridRef " << guid << " as reference surface" << std::endl;
        surface = ctx->LookupSurface(guid);
        if ( surface.IsNull()) {
            std::cerr << "failed to lookup grid reference to " << guid << " contained in panel " << id << std::endl;
            return panelAssy;
        }
    } else if ( ! surfaceRefN.isNull()) {
        std::string guid = std::string(surfaceRefN.getAttribute( ctx->OCXGUIDRef()).GetString());
        std::cout << "use SurfaceRef " << guid << " as reference surface" << std::endl;
        surface = ctx->LookupSurface(guid);
        if ( surface.IsNull()) {
            std::cerr << "failed to lookup surface reference to " << guid << " contained in panel " << id << std::endl;
            return panelAssy;
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
                        return panelAssy;
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
     } */

    TopoDS_Compound panelAssy;
    BRep_Builder compoundBuilder;
    compoundBuilder.MakeCompound (panelAssy);
    for( TopoDS_Shape shape : shapes) {
        compoundBuilder.Add(panelAssy, shape);
    }

    TDF_Label panelLabel = ctx->GetOCAFShapeTool()->AddShape(panelAssy, true);
    TDataStd_Name::Set(panelLabel, ( name + " (" + id + ")").c_str());

    return panelAssy;

}

