//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <TopoDS.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include "OCXSurfaceReader.h"
#include "OCXHelper.h"
#include "OCXCurveReader.h"

OCXSurfaceReader::OCXSurfaceReader(OCXContext *ctx) : ctx(ctx) {
    this->ctx = ctx;
}

TopoDS_Shape OCXSurfaceReader::ReadSurface(LDOM_Element &surfaceN) {

    std::string guid = std::string(surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString());
    std::string id = std::string(surfaceN.getAttribute("id").GetString());

    try {
        if ("SurfaceCollection" == OCXHelper::GetLocalTagName(surfaceN)) {
            return ReadSurfaceCollection(surfaceN, guid, id);
        } else {

            if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadCone3D(surfaceN, guid, id);
            } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadCylinder3D(surfaceN, guid, id);
            } else if ("ReadExtrudedSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadExtrudedSurface(surfaceN, guid, id);
            } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadNURBSurface(surfaceN, guid, id);
            } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadPlane3D(surfaceN, guid, id);
            } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                return ReadSphere3D(surfaceN, guid, id);
            }
            std::cerr << "found unknown surface type " << surfaceN.getTagName().GetString() << ", guid (" << guid
                      << "), id " << id << std::endl;
            return TopoDS_Shell();

        }
    } catch (Standard_Failure exp) {
        std::cerr << "an error occurred reading surface "
                  << surfaceN.getAttribute("id").GetString() << ":" << exp << std::endl;
        return TopoDS_Shell();
    }
}

TopoDS_Face OCXSurfaceReader::ReadCone3D(LDOM_Element &surfaceN, std::string guid, std::string id) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadCylinder3D(LDOM_Element &surfaceN, std::string guid, std::string id) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadExtrudedSurface(LDOM_Element &surfaceN, std::string guid, std::string id) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadNURBSurface(LDOM_Element &nurbN, std::string guid, std::string id) {
    std::string colGuid = std::string(nurbN.getAttribute(ctx->OCXGUIDRef()).GetString());

    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadSphere3D(LDOM_Element &surfaceN, std::string guid, std::string id) {
    return TopoDS_Face();
}

TopoDS_Shell OCXSurfaceReader::ReadSurfaceCollection(LDOM_Element &surfColN, std::string guid, std::string id) {

    std::string colGuid = std::string(surfColN.getAttribute(ctx->OCXGUIDRef()).GetString());

    TopoDS_Shell shell = TopoDS_Shell();
    // see https://dev.opencascade.org/content/brepbuilderapimakeshell
    BRepOffsetAPI_Sewing sewedObj;

    LDOM_Node childN = surfColN.getFirstChild();
    while (childN != NULL) {
        const LDOM_Node::NodeType nodeType = childN.getNodeType();
        if (nodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (nodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element surfaceN = (LDOM_Element &) childN;

            std::string guid = std::string(surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString());
            std::string id = std::string(surfaceN.getAttribute("id").GetString());

            std::cout << "    SurfaceCollection/" << OCXHelper::GetLocalTagName(surfaceN)
                      << " guid=" << guid << ", id=" << id << std::endl;

            TopoDS_Face face = TopoDS_Face();
            if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadCone3D(surfaceN, guid, id);
            } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadCylinder3D(surfaceN, guid, id);
            } else if ("ReadExtrudedSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadExtrudedSurface(surfaceN, guid, id);
            } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadNURBSurface(surfaceN, guid, id);
            } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadPlane3D(surfaceN, guid, id);
            } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = ReadSphere3D(surfaceN, guid, id);
            } else {
                std::cerr << "found unknown face type " << surfaceN.getTagName().GetString()
                          << " in SurfaceCollection with guid '" << colGuid << "'" << std::endl;
                continue;
            }

            if (face.IsNull()) {
                std::cerr << "failed to read from face " << surfaceN.getTagName().GetString()
                          << " (" << guid << ") in SurfaceCollection with guid '" << colGuid << "'" << std::endl;
            } else {
                sewedObj.Add(face);
            }
        }
        childN = childN.getNextSibling();
    }

    sewedObj.Perform();
    TopoDS_Shape rawSewer = sewedObj.SewedShape();
    TopExp_Explorer explorer(rawSewer, TopAbs_SHELL);

    int numShells = 0;
    if (explorer.More()) {

        TopoDS_Shape aTmpShape = explorer.Current();
        shell = TopoDS::Shell(aTmpShape);
        numShells++;
    }
    if (numShells != 1) {
        std::cerr << "expected one shell from failed to read from SurfaceCollection "
                  << " (" << colGuid << "), got #" << numShells << ", use only last" << std::endl;
    }


    return shell;

}

TopoDS_Face OCXSurfaceReader::ReadPlane3D(LDOM_Element &surfaceN, std::string guid, std::string id) {
    LDOM_Element originN = OCXHelper::GetFirstChild(surfaceN, "Origin");
    if (originN.isNull()) {
        std::cout << "could not read surface due to missing Plane3D/Origin element, id '" << guid << "'" << std::endl;
        return TopoDS_Face();
    }
    gp_Pnt origin = OCXHelper::ReadPoint(originN, ctx);

    LDOM_Element normalN = OCXHelper::GetFirstChild(surfaceN, "Normal");
    if (normalN.isNull()) {
        std::cout << "could not read surface due to missing Plane3D/Normal element,  GUID is '" << guid << "'"
                  << std::endl;
        return TopoDS_Face();
    }
    gp_Dir normal = OCXHelper::ReadDirection(normalN);

    // UDirection, FaceBoundaryCurve and LimitedBy are not mandatory in schema
    // TODO: read none mandatory elements from Plane3D

    gp_Pln plane = gp_Pln(origin, normal);
    TopoDS_Face planeFace = BRepBuilderAPI_MakeFace(plane);

    LDOM_Element faceBoundaryCurveN = OCXHelper::GetFirstChild(surfaceN, "FaceBoundaryCurve");
    if (faceBoundaryCurveN.isNull()) {
        std::cout << "could not find  Plane3D/FaceBoundaryCurve in element with (" << guid << ")" << std::endl;
        return planeFace;
    }

    OCXCurveReader *curveReader = new OCXCurveReader(ctx);
    TopoDS_Wire outerContour = curveReader->ReadCurve(faceBoundaryCurveN);

    BRepBuilderAPI_MakeFace faceBuilder = BRepBuilderAPI_MakeFace(planeFace, outerContour);
    faceBuilder.Build();
    TopoDS_Face restricted = faceBuilder.Face();
    if (restricted.IsNull()) {
        std::cerr << "failed to create a restricted surface from a Plane3D and Plane3D/FaceBoundaryCurveur, guid "
                  << guid << ", id " << id << ", error " << faceBuilder.Error() << std::endl;
        restricted = TopoDS_Face();
    }

    return restricted;
}
