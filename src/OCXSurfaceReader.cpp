/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#include "OCXSurfaceReader.h"

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>

#include "OCXCurveReader.h"
#include "OCXHelper.h"

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

TopoDS_Face OCXSurfaceReader::ReadNURBSurface(LDOM_Element &nurbsSrfN, std::string guid, std::string id) {

    std::string guid = std::string(nurbsSrfN.getAttribute(ctx->OCXGUID()).GetString());
    std::string id = std::string(nurbsSrfN.getAttribute("id").GetString());

    LDOM_Element uPropsN = OCXHelper::GetFirstChild(nurbsSrfN, "U_NURBSproperties" );
    if ( uPropsN.isNull()) {
        std::cerr << "could nit find failed NURBSSurface/U_NURBSproperties in surface with id " << id <<", GUID" << guid << std::endl;
        return TopoDS_Face();
    }
    LDOM_Element vPropsN = OCXHelper::GetFirstChild(nurbsSrfN, "V_NURBSproperties" );
    if ( vPropsN.isNull()) {
        std::cerr << "could nit find failed NURBSSurface/V_NURBSproperties in surface with id " << id <<", GUID" << guid << std::endl;
        return TopoDS_Face();
    }

    // U properties
    int uDegree = -1;
    int uNumCtrlPoints = -1;
    int uNumKnots =-1;
    uPropsN.getAttribute("degree").GetInteger(uDegree);
    uPropsN.getAttribute("numCtrlPts").GetInteger(uNumCtrlPoints);
    uPropsN.getAttribute("numKnots").GetInteger(uNumKnots);
    std::string uForm= std::string( uPropsN.getAttribute("form").GetString());
    bool uIsRational = "true".IsEqual( std::string( uPropsN.getAttribute("isRational").GetString()));

    std::cout << "U degree " << uDegree << ", #ctr " << uNumCtrlPoints << ", #knots " << uNumKnots << ", form '" << uForm <<
              (uIsRational ? "', rational" : "', irrational") << std::endl;

    // V properties
    int vDegree = -1;
    int vNumCtrlPoints = -1;
    int vNumKnots =-1;
    vPropsN.getAttribute("degree").GetInteger(vDegree);
    vPropsN.getAttribute("numCtrlPts").GetInteger(vNumCtrlPoints);
    vPropsN.getAttribute("numKnots").GetInteger(vNumKnots);
    std::string vForm= std::string( vPropsN.getAttribute("form").GetString());
    bool vIsRational = "true".IsEqual( std::string( vPropsN.getAttribute("isRational").GetString()));

    std::cout << "V degree " << vDegree << ", #ctr " << vNumCtrlPoints << ", #knots " << vNumKnots << ", form '" << vForm <<
              (vIsRational ? "', rational" : "', irrational") << std::endl;


    //
    // the knot vectors
    //
    LDOM_Element uKnotVectorN = OCXHelper::GetFirstChild(nurbsSrfN, "UKnotVector");
    if (uKnotVectorN.isNull()) {
        std::cout << "could not find NURBSSurface/UKnotVector in surface id='" << id << "'" << std::endl;
        return TopoDS_Face();
    }

    std::string uKnotVectorS = std::string(uKnotVectorN.getAttribute("value").GetString());

    KnotMults uKN = OCXHelper::ParseKnotVector( uKnotVectorS, uNumKnots);
    if ( uKN.IsNull) {
        return TopoDS_Face();
    }
    LDOM_Element vKnotVectorN = OCXHelper::GetFirstChild(nurbsSrfN, "VKnotVector");
    if (uKnotVectorN.isNull()) {
        std::cout << "could not find NURBSSurface/VKnotVector in surface id='" << id << "'" << std::endl;
        return TopoDS_Face();
    }

    std::string vKnotVectorS = std::string(vKnotVectorN.getAttribute("value").GetString());

    KnotMults vKN = OCXHelper::ParseKnotVector( vKnotVectorS, vNumKnots);
    if ( vKN.IsNull) {
        return TopoDS_Face();
    }

    //
    // the control points
    //
    LDOM_Element controlPtListN = OCXHelper::GetFirstChild(nurbsSrfN, "ControlPtList");
    if (controlPtListN.isNull()) {
        std::cout << "could not find NURBSSurface/ControlPtList in surface id='" << id << "'" << std::endl;
        return TopoDS_Face();
    }
    PolesWeights pw = OCXHelper::ParseControlPoints( controlPtListN, uNumKnots, vNumKnots);
    if ( pw.IsNull) {
        return TopoDS_Face();
    }

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
            LDOM_Element surfaceN = (LDOM_Element &)childN;

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
