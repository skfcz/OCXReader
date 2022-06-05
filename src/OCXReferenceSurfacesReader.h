//
// Created by cz on 04.06.22.
//

#ifndef OCXREADER_OCXREFERENCESURFACESREADER_H
#define OCXREADER_OCXREFERENCESURFACESREADER_H


#include "OCXContext.h"

class OCXReferenceSurfacesReader {
public:
    OCXReferenceSurfacesReader( OCXContext * ctx);
    Standard_Boolean ReadReferenceSurfaces( LDOM_Element & vesselN);

private:
    OCXContext * ctx;

};


#endif //OCXREADER_OCXREFERENCESURFACESREADER_H
