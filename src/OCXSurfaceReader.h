//
// Created by cz on 04.06.22.
//

#ifndef OCXREADER_OCXSURFACEREADER_H
#define OCXREADER_OCXSURFACEREADER_H


#include "OCXContext.h"

class OCXSurfaceReader {
public:
    OCXSurfaceReader(OCXContext *ctx);
    Standard_Boolean Read( LDOM_Element & vesselN );

private:
    OCXContext * ctx;
};


#endif //OCXREADER_OCXSURFACEREADER_H
