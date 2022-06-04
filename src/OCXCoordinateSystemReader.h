//
// Created by cz on 04.06.22.
//

#ifndef OCXREADER_OCXCOORDINATESYSTEMREADER_H
#define OCXREADER_OCXCOORDINATESYSTEMREADER_H


#include "OCXContext.h"

class OCXCoordinateSystemReader {

public:
    OCXCoordinateSystemReader( OCXContext * ctx);
    Standard_Boolean ReadCoordinateSystem( LDOM_Element & vesselN);

private:
    OCXContext * ctx;

    Standard_Boolean ReadRefPlanes(LDOM_Element & refPlanesN);

};


#endif //OCXREADER_OCXCOORDINATESYSTEMREADER_H
