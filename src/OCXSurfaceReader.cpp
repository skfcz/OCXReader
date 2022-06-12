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
#include "OCXSurfaceReader.h"
#include "OCXHelper.h"

OCXSurfaceReader::OCXSurfaceReader(OCXContext *ctx) : ctx(ctx) {
    this->ctx = ctx;
}

TopoDS_Shape OCXSurfaceReader::ReadSurface(LDOM_Element &surfaceN) {
    TopoDS_Shape surface = TopoDS_Shape();

    std::string guid = std::string( surfaceN.getAttribute( ctx->OCXGUIDRef()).GetString());

    if ("SurfaceCollection" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadSurfaceCollection( surfaceN, guid);
    } else if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadCone3D( surfaceN, guid);
    } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadCylinder3D(surfaceN, guid);
    } else if ("ReadExtrudedSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadExtrudedSurface(surfaceN, guid);
    } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadNURBSurface(surfaceN, guid);
    } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadPlane3D(surfaceN, guid);
    } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
        surface = ReadSphere3D(surfaceN, guid);
    } else {
        std::cerr << "found unknown surface type " << surfaceN.getTagName().GetString() << " with guid '" << guid << "'" << std::endl;
    }
    return surface;
}

TopoDS_Face OCXSurfaceReader::ReadCone3D(LDOM_Element &surfaceN, std::string guid) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadCylinder3D(LDOM_Element &surfaceN, std::string guid) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadExtrudedSurface(LDOM_Element &surfaceN, std::string guid) {
    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadNURBSurface(LDOM_Element &nurbN, std::string guid) {
    TopoDS_Face surface = TopoDS_Face();
    std::string colGuid = std::string( nurbN.getAttribute( ctx->OCXGUIDRef()).GetString());

    return TopoDS_Face();
}

TopoDS_Face OCXSurfaceReader::ReadSphere3D(LDOM_Element &surfaceN, std::string guid) {
    return TopoDS_Face();
}

TopoDS_Shape OCXSurfaceReader::ReadSurfaceCollection(LDOM_Element &surfColN, std::string guid) {

    std::string colGuid = std::string( surfColN.getAttribute( ctx->OCXGUIDRef()).GetString());


    BRepOffsetAPI_Sewing sewer;
    sewer.SetTolerance(0.5);
    sewer.SetMaxTolerance(1.0);
    sewer.SetMinTolerance(0.1);

    LDOM_Node childN = surfColN.getFirstChild();

    while (childN != NULL) {
        const LDOM_Node::NodeType nodeType = childN.getNodeType();
        if (nodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (nodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element surfaceN = (LDOM_Element &) childN;

            std::string guid = std::string(surfaceN.getAttribute(ctx->OCXGUIDRef()).GetString());

            std::cout << "    SurfaceCollection/" << OCXHelper::GetLocalTagName(surfaceN) << " guid=" << guid
                      << std::endl;

            if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face   surface = ReadCone3D(surfaceN, guid);
                sewer.Add( surface);
            } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face   surface = ReadCylinder3D(surfaceN, guid);
                sewer.Add( surface);
            } else if ("ReadExtrudedSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face        surface = ReadExtrudedSurface(surfaceN, guid);
                sewer.Add( surface);
            } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face           surface = ReadNURBSurface(surfaceN, guid);
                sewer.Add( surface);
            } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face            surface = ReadPlane3D(surfaceN, guid);
                sewer.Add( surface);
            } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                TopoDS_Face         surface = ReadSphere3D(surfaceN, guid);
                sewer.Add( surface);
            } else {
                std::cerr << "found unknown surface type " << surfaceN.getTagName().GetString()
                          << " in SurfaceCollection with guid '" << colGuid << "'" << std::endl;
            }
        }
        childN = childN.getNextSibling();
    }
    sewer.Perform();
    TopoDS_Shape shape =  sewer.SewedShape();
    return shape;

}

TopoDS_Face OCXSurfaceReader::ReadPlane3D(LDOM_Element &surfaceN, std::string guid) {
    LDOM_Element originN = OCXHelper::GetFirstChild(surfaceN, "Origin");
    if ( originN.isNull()) {
        std::cout << "could not read surface due to missing Plane3D/Origin element,  GUID is '"<< guid << "'" <<  std::endl;
        return TopoDS_Face();
    }
    gp_Pnt origin = OCXHelper::ReadPoint( originN, ctx);

    LDOM_Element normalN = OCXHelper::GetFirstChild(surfaceN, "Normal");
    if ( normalN.isNull()) {
        std::cout << "could not read surface due to missing Plane3D/Normal element,  GUID is '"<< guid << "'" <<  std::endl;
        return TopoDS_Face();
    }
    gp_Dir normal = OCXHelper::ReadDirection( normalN);

    // UDirection, FaceBoundaryCurve and LimitedBy are not mandatory in schema
    // TODO: read none mandatory elements from Plane3D

    gp_Pln plane =gp_Pln( origin, normal);
    TopoDS_Face planeFace = BRepBuilderAPI_MakeFace(plane);

    return planeFace;
}
