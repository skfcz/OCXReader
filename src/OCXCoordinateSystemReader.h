//
// Created by cz on 04.06.22.
//

#ifndef OCXREADER_OCXCOORDINATESYSTEMREADER_H
#define OCXREADER_OCXCOORDINATESYSTEMREADER_H


#include "OCXContext.h"
#include <TopoDS_Shape.hxx>

class OCXCoordinateSystemReader {

public:
    OCXCoordinateSystemReader( OCXContext * ctx);
    TopoDS_Shape ReadCoordinateSystem( LDOM_Element & vesselN);

private:
    OCXContext * ctx;

    TopoDS_Shape ReadRefPlanes(LDOM_Element & refPlanesN);

};


#endif //OCXREADER_OCXCOORDINATESYSTEMREADER_H
