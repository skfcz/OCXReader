/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXCOORDINATESYSTEMREADER_H
#define OCXREADER_OCXCOORDINATESYSTEMREADER_H

#include <TopoDS_Shape.hxx>

#include "OCXContext.h"

class OCXCoordinateSystemReader {
   public:
    OCXCoordinateSystemReader(OCXContext* ctx);
    TopoDS_Shape ReadCoordinateSystem(LDOM_Element& vesselN);

   private:
    OCXContext* ctx;

    TopoDS_Shape ReadRefPlanes(LDOM_Element& refPlanesN);
};

#endif  // OCXREADER_OCXCOORDINATESYSTEMREADER_H
