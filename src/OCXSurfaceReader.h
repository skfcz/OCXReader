//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//


#ifndef OCXREADER_OCXSURFACEREADER_H
#define OCXREADER_OCXSURFACEREADER_H


#include "OCXContext.h"

class OCXSurfaceReader {
public:
    OCXSurfaceReader(OCXContext *ctx);
    TopoDS_Shape  ReadSurface( LDOM_Element & vesselN );

private:
    OCXContext * ctx;

    TopoDS_Face  ReadCone3D(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Face  ReadCylinder3D(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Face  ReadExtrudedSurface(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Face  ReadNURBSurface(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Face  ReadPlane3D(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Face  ReadSphere3D(LDOM_Element & surfaceN, std::string guid, std::string id);
    TopoDS_Shell  ReadSurfaceCollection(LDOM_Element & surfaceN, std::string guid, std::string id);
};


#endif //OCXREADER_OCXSURFACEREADER_H
