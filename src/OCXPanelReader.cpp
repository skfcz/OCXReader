/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#include "OCXPanelReader.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "OCXCurveReader.h"
#include "OCXHelper.h"
#include "OCXSurfaceReader.h"

OCXPanelReader::OCXPanelReader(OCXContext *ctx) {
    this->ctx = ctx;
}

TopoDS_Shape OCXPanelReader::ReadPanels(LDOM_Element &vesselN) {
    std::list<TopoDS_Shape> shapes;

    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = vesselN.getFirstChild();
    while (aChildNode != NULL) {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element aNextElement = (LDOM_Element &)aChildNode;
            if ("Panel" == OCXHelper::GetLocalTagName(aNextElement)) {
                TopoDS_Shape shape = ReadPanel(aNextElement);
                if (!shape.IsNull()) {
                    shapes.push_back(shape);
                }
            }
        }
        aChildNode = aChildNode.getNextSibling();
    }

    TopoDS_Compound panelsAssy;
    BRep_Builder panelsBuilder;
    panelsBuilder.MakeCompound(panelsAssy);

    for (TopoDS_Shape shape : shapes) {
        panelsBuilder.Add(panelsAssy, shape);
    }

    return panelsAssy;
}

TopoDS_Shape OCXPanelReader::ReadPanel(LDOM_Element &panelN) {
    std::string id = std::string(panelN.getAttribute("id").GetString());
    std::string name = std::string(panelN.getAttribute("name").GetString());

    std::cout << "parse Panel " << id << ", name='" << name << "'" << std::endl;

    std::list<TopoDS_Shape> shapes;
    Quantity_Color contourColor = Quantity_Color(20 / 256.0, 20 / 256.0, 20 / 256, Quantity_TOC_RGB);
    // Material Design Light Green 50 300
    Quantity_Color surfaceColor = Quantity_Color(174 / 256.0, 213 / 256.0, 129 / 256, Quantity_TOC_RGB);

    TopoDS_Wire outerContour = ReadPanelOuterContour(panelN);

    if (!outerContour.IsNull() && OCXContext::CreatePanelContours) {
        TDF_Label label = ctx->OCAFShapeTool()->AddShape(outerContour, false);
        TDataStd_Name::Set(label, (id + " Contour").c_str());
        ctx->OCAFColorTool()->SetColor(label, contourColor, XCAFDoc_ColorCurv);

        shapes.push_back(outerContour);
    }
    std::cout << "    finished outerContour" << std::endl;

    TopoDS_Shape panelSurface = TopoDS_Shape();
    if (OCXContext::CreatePanelSurfaces) {
        panelSurface = ReadPanelSurface(panelN, outerContour);
        if (!panelSurface.IsNull()) {
            TDF_Label label = ctx->OCAFShapeTool()->AddShape(panelSurface, false);
            TDataStd_Name::Set(label, (id + " Surface").c_str());
            ctx->OCAFColorTool()->SetColor(label, surfaceColor, XCAFDoc_ColorSurf);

            shapes.push_back(panelSurface);
        }
    }

    std::cout << "    finished panelsurface" << std::endl;

    if (OCXContext::CreatePlateSurfaces) {
        if (panelSurface.IsNull()) {
            std::cerr << "failed to read  panelsurface in Panel " << id << ", name='" << name << "'," << std::endl;

        } else {
            TopoDS_Shape plates = ReadPlates(panelN, panelSurface);
            if (!plates.IsNull()) {
                TDF_Label label = ctx->OCAFShapeTool()->AddShape(plates, false);
                TDataStd_Name::Set(label, (id + " Plates").c_str());
                shapes.push_back(plates);
            }
        }
    }
    if (OCXContext::CreateStiffenerTraces) {
        TopoDS_Shape stiffeners = ReadStiffeners(panelN);
        if (!stiffeners.IsNull()) {
            TDF_Label label = ctx->OCAFShapeTool()->AddShape(stiffeners, false);
            TDataStd_Name::Set(label, (id + " Stiffeners").c_str());
            shapes.push_back(stiffeners);
        }
    }

    TopoDS_Compound panelAssy;
    BRep_Builder compoundBuilder;
    compoundBuilder.MakeCompound(panelAssy);
    for (TopoDS_Shape shape : shapes) {
        compoundBuilder.Add(panelAssy, shape);
    }

    TDF_Label panelLabel = ctx->OCAFShapeTool()->AddShape(panelAssy, true);
    TDataStd_Name::Set(panelLabel, (name + " (" + id + ")").c_str());

    return panelAssy;
}

TopoDS_Wire OCXPanelReader::ReadPanelOuterContour(LDOM_Element &panelN) {
    TopoDS_Wire outerContour = TopoDS_Wire();

    LDOM_Element outerContourN = OCXHelper::GetFirstChild(panelN, "OuterContour");
    if (outerContourN.isNull()) {
        std::cerr << "could not find OuterContour for Panel " << panelN.getAttribute("id").GetString() << std::endl;
        return outerContour;
    }
    try {
        OCXCurveReader *reader = new OCXCurveReader(ctx);
        TopoDS_Shape curveShape = reader->ReadCurve(outerContourN);
        if (curveShape.ShapeType() != TopAbs_WIRE) {
            std::cerr << "expect a closed shape as OuterContour, but got " << curveShape.ShapeType() << " in "
                      << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        }
        return TopoDS::Wire(curveShape);

    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring OuterContour from panel "
                  << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        return outerContour;
    }
}

TopoDS_Shape OCXPanelReader::ReadPanelSurface(LDOM_Element &panelN, TopoDS_Wire &outerContour) {
    const char *id = panelN.getAttribute("id").GetString();

    TopoDS_Shape shell = TopoDS_Shape();

    LDOM_Element unboundedGeometryN = OCXHelper::GetFirstChild(panelN, "UnboundedGeometry");
    if (unboundedGeometryN.isNull()) {
        std::cerr << "could not find UnboundedGeometry for Panel " << id << std::endl;
        return shell;
    }

    // The unbounded geometry is either a shell, or a shell reference, or a grid reference
    try {
        LDOM_Element gridRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");
        LDOM_Element surfaceRefN = OCXHelper::GetFirstChild(unboundedGeometryN, "GridRef");

        if (!gridRefN.isNull()) {
            std::string guid = std::string(gridRefN.getAttribute(ctx->OCXGUIDRef()).GetString());

            std::cout << "use GridRef " << guid << " as reference shell" << std::endl;
            shell = ctx->LookupSurface(guid);
            if (shell.IsNull()) {
                std::cerr << "failed to lookup grid reference to " << guid << " contained in panel " << id << std::endl;
                return shell;
            }
            // TODO: shell from GridRef
            return shell;
        }
        if (!surfaceRefN.isNull()) {
            std::string guid = std::string(surfaceRefN.getAttribute(ctx->OCXGUIDRef()).GetString());
            std::cout << "use SurfaceRef " << guid << " as reference shell" << std::endl;
            shell = ctx->LookupSurface(guid);
            if (shell.IsNull()) {
                std::cerr << "failed to lookup shell reference to " << guid << " contained in panel " << id
                          << std::endl;
                return shell;
            }
            // TODO: restricted shell from surfaceReference
            return shell;
        }

        LDOM_Node aChildNode = unboundedGeometryN.getFirstChild();
        while (aChildNode != NULL) {
            const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
            if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
                break;
            if (aNodeType == LDOM_Node::ELEMENT_NODE) {
                LDOM_Element surfaceN = (LDOM_Element &)aChildNode;
                OCXSurfaceReader *surfaceReader = new OCXSurfaceReader(ctx);
                shell = surfaceReader->ReadSurface(surfaceN);
                if (shell.IsNull()) {
                    std::cerr << "failed to read shell definition from Panel/UnboundedGeometry contained in panel "
                              << id << std::endl;
                }
                return shell;  // we expect only one child element
            }
            aChildNode = aChildNode.getNextSibling();
        }
        std::cerr << "failed to find geometry Panel/UnboundedGeometry contained in panel "
                  << id << std::endl;
        return shell;
    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred reading panel shell from panel "
                  << panelN.getAttribute("id").GetString() << ":" << exp << std::endl;
        return shell;
    }
}

TopoDS_Shape OCXPanelReader::ReadPlates(LDOM_Element &panelN, TopoDS_Shape &referenceSurface) {
    const char *id = panelN.getAttribute("id").GetString();

    TopoDS_Shape plates = TopoDS_Shape();

    LDOM_Element composedOfN = OCXHelper::GetFirstChild(panelN, "ComposedOf");
    if (composedOfN.isNull()) {
        std::cerr << "could not find ComposedOf for Panel " << id << std::endl;
        return plates;
    }
    std::list<TopoDS_Shape> shapes;

    LDOM_Node childN = composedOfN.getFirstChild();
    while (childN != NULL) {
        const LDOM_Node::NodeType aNodeType = childN.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element plateN = (LDOM_Element &)childN;
            if ("Plate" == OCXHelper::GetLocalTagName(plateN)) {
                TopoDS_Shape plate = ReadPlate(plateN, referenceSurface);
                if (!plate.IsNull()) {
                    shapes.push_back(plate);
                }
                std::cout << "finished plate" << std::endl;
            }
        }
        childN = childN.getNextSibling();
    }
    std::cout << "finished plates loop" << std::endl;

    TopoDS_Compound plateAssy;
    BRep_Builder compoundBuilder;
    compoundBuilder.MakeCompound(plateAssy);
    for (TopoDS_Shape shape : shapes) {
        compoundBuilder.Add(plateAssy, shape);
    }

    TDF_Label panelLabel = ctx->OCAFShapeTool()->AddShape(plateAssy, true);
    TDataStd_Name::Set(panelLabel, "Plates");

    std::cout << "finished plates" << std::endl;

    return plateAssy;
}

TopoDS_Shape OCXPanelReader::ReadPlate(LDOM_Element &plateN, TopoDS_Shape &referenceSurface) {
    std::string guid = std::string(plateN.getAttribute(ctx->OCXGUIDRef()).GetString());
    std::string id = std::string(plateN.getAttribute("id").GetString());

    std::cout << "ReadPlate, id " << id << " (" << guid << ")" << std::endl;

    TopoDS_Shape plateShape = TopoDS_Shape();

    LDOM_Element outerContourN = OCXHelper::GetFirstChild(plateN, "OuterContour");
    if (outerContourN.isNull()) {
        std::cerr << "could not find OuterContour for Plate " << id << " (" << guid << ")" << std::endl;
        return plateShape;
    }

    // material design green 50
    Quantity_Color plateColor = Quantity_Color(76 / 255.0, 175 / 255.0, 80 / 255.0, Quantity_TOC_RGB);

    try {
        OCXCurveReader *reader = new OCXCurveReader(ctx);
        TopoDS_Shape curveShape = reader->ReadCurve(outerContourN);
        if (curveShape.ShapeType() != TopAbs_WIRE) {
            std::cerr << "expect a closed shape as OuterContour, but got " << curveShape.ShapeType() << " in plate "
                      << id << " (" << guid << ")" << std::endl;
            return plateShape;
        }
        TopoDS_Wire outerContour = TopoDS::Wire(curveShape);

        std::cout << "    finished Plate/OuterContour" << std::endl;

        if (referenceSurface.ShapeType() == TopAbs_FACE) {
            TopoDS_Face face = TopoDS::Face(referenceSurface);
            BRepBuilderAPI_MakeFace faceBuilder = BRepBuilderAPI_MakeFace(face, outerContour);
            faceBuilder.Build();
            TopoDS_Face restricted = faceBuilder.Face();
            if (restricted.IsNull()) {
                std::cerr
                    << "failed to create a restricted surface Panel/UnboundedGeometry + Plate/OuterContour contained in panel "
                    << id << ", plate " << id << " (" << guid << ") : " << faceBuilder.Error() << std::endl;
                return plateShape;
            }
            std::cout << "    created plate surface" << std::endl;

            TDF_Label label = ctx->OCAFShapeTool()->AddShape(restricted, true);
            TDataStd_Name::Set(label, ("Plate " + id + " (" + guid + ")").c_str());
            ctx->OCAFColorTool()->SetColor(label, plateColor, XCAFDoc_ColorSurf);

            return restricted;

        } else {
            // TODO: limit the faces in the shell by the outer contour
        }

        return plateShape;

    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred transferring OuterContour from plate  "
                  << id << " (" << guid << ") : " << exp << std::endl;
        return TopoDS_Shape();
    }
}

TopoDS_Shape OCXPanelReader::ReadStiffeners(LDOM_Element &panelN) {
    const char *id = panelN.getAttribute("id").GetString();

    TopoDS_Shape stiffeners = TopoDS_Shape();

    LDOM_Element composedOfN = OCXHelper::GetFirstChild(panelN, "StiffenedBy");
    if (composedOfN.isNull()) {
        std::cout << "could not find StiffenedBy for Panel " << id << std::endl;
        return stiffeners;
    }
    std::list<TopoDS_Shape> shapes;

    LDOM_Node childN = composedOfN.getFirstChild();
    while (childN != NULL) {
        const LDOM_Node::NodeType aNodeType = childN.getNodeType();
        if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (aNodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element stiffenerN = (LDOM_Element &)childN;
            if ("Stiffener" == OCXHelper::GetLocalTagName(stiffenerN)) {
                TopoDS_Shape stiffener = ReadStiffener(stiffenerN);
                if (!stiffener.IsNull()) {
                    shapes.push_back(stiffener);
                }
            }
        }
        childN = childN.getNextSibling();
    }
    std::cout << "finished stiffeners loop, found #" << shapes.size() << " stiffeners" << std::endl;
    if (shapes.size() == 0) {
        return stiffeners;
    }

    TopoDS_Compound stiffenersAssy;
    BRep_Builder compoundBuilder;
    compoundBuilder.MakeCompound(stiffenersAssy);
    for (TopoDS_Shape shape : shapes) {
        compoundBuilder.Add(stiffenersAssy, shape);
    }

    TDF_Label panelLabel = ctx->OCAFShapeTool()->AddShape(stiffenersAssy, true);
    TDataStd_Name::Set(panelLabel, "Stiffeners");

    std::cout << "finished stiffeners" << std::endl;

    return stiffenersAssy;
}

TopoDS_Shape OCXPanelReader::ReadStiffener(LDOM_Element &stiffenerN) {
    std::string guid = std::string(stiffenerN.getAttribute(ctx->OCXGUIDRef()).GetString());
    std::string id = std::string(stiffenerN.getAttribute("id").GetString());

    std::cout << "ReadStiffener, id " << id << " (" << guid << ")" << std::endl;

    try {
        // material design red 50 500
        Quantity_Color plateColor = Quantity_Color(244 / 255.0, 67 / 255.0, 54 / 255.0, Quantity_TOC_RGB);

        LDOM_Element traceN = OCXHelper::GetFirstChild(stiffenerN, "TraceLine");
        if (traceN.isNull()) {
            std::cerr << "could not find Stiffener/TraceLine element in stiffener with id " << id << ", guid (" << guid
                      << ")" << std::endl;
        } else {
            OCXCurveReader *curveReader = new OCXCurveReader(ctx);
            TopoDS_Shape trace = curveReader->ReadCurve(traceN);

            if (trace.IsNull()) {
                std::cerr << "failed to read Stiffener/TraceLine element in stiffener with id " << id << ", guid ("
                          << guid
                          << ")" << std::endl;
            } else {
                TDF_Label label = ctx->OCAFShapeTool()->AddShape(trace, false);
                TDataStd_Name::Set(label, ("Stiffener " + id + " (" + guid + ")").c_str());
                ctx->OCAFColorTool()->SetColor(label, plateColor, XCAFDoc_ColorSurf);
                return trace;
            }
        }
        // TODO: Add simplified geometry representation of stiffener

    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred reading stiffener  "
                  << id << " (" << guid << ") : " << exp << std::endl;
        return TopoDS_Shape();
    }
    return TopoDS_Shape();
}

TopoDS_Shape OCXPanelReader::ReadBrackets(LDOM_Element &panelN) {
    return TopoDS_Shape();
}

TopoDS_Shape OCXPanelReader::ReadBracket(LDOM_Element &bracketN) {
    return TopoDS_Shape();
}
