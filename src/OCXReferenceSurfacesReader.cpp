//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "OCXReferenceSurfacesReader.h"
#include "OCXHelper.h"
#include "OCXSurfaceReader.h"

OCXReferenceSurfacesReader::OCXReferenceSurfacesReader(OCXContext *ctx) {
    this->ctx = ctx;

}

Standard_Boolean OCXReferenceSurfacesReader::ReadReferenceSurfaces(LDOM_Element &vesselN) {

    std::string tag = std::string(vesselN.getTagName().GetString());
    std::size_t index = tag.find("Vessel");
    if (index == std::string::npos) {
        std::cout << "expected a Vessel element, got " << tag << std::endl;
        return false;
    }

    LDOM_Element refSrfsN = OCXHelper::GetFirstChild(vesselN, "ReferenceSurfaces");
    if (refSrfsN.isNull()) {
        std::cout << "could not find ReferenceSurfaces child node" << std::endl;
        return false;
    }

    OCXSurfaceReader * surfaceReader = new OCXSurfaceReader(ctx);

    LDOM_Node childN = refSrfsN.getFirstChild();

    while (childN != NULL) {
        const LDOM_Node::NodeType nodeType = childN.getNodeType();
        if (nodeType == LDOM_Node::ATTRIBUTE_NODE)
            break;
        if (nodeType == LDOM_Node::ELEMENT_NODE) {
            LDOM_Element surfaceN = (LDOM_Element &) childN;

            std::string guid = std::string( surfaceN.getAttribute( ctx->OCXGUIDRef()).GetString());

            std::cout << "read reference surface " << surfaceN.getTagName().GetString() << " guid=" << guid << std::endl;

            TopoDS_Face face = TopoDS_Face();

            if ("SurfaceCollection" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("Cone3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("Cylinder3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("ReadExtrudedSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("NURBSSurface" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("Plane3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else if ("Sphere3D" == OCXHelper::GetLocalTagName(surfaceN)) {
                face = surfaceReader->ReadSurface(surfaceN);
            } else {
                std::cerr << "found unknown face type " << surfaceN.getTagName().GetString() << " with guid '" << guid << "'" << std::endl;
            }

            if ( ! face.IsNull()) {
                ctx->RegisterSurface(guid, face);
            } else {
                std::cerr << "could not read surface with guid '" << guid << "'" << std::endl;
            }
        }
        childN = childN.getNextSibling();
    }


    return true;
}
