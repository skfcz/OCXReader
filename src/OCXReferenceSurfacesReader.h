//
// Created by cz on 04.06.22.
//

#ifndef OCXREADER_OCXREFERENCESURFACESREADER_H
#define OCXREADER_OCXREFERENCESURFACESREADER_H


#include "OCXContext.h"

class OCXReferenceSurfacesReader {
public:
    OCXReferenceSurfacesReader( OCXContext * ctx);

    /**
     * Read the references surfaces from the OCX file.
     * The references surfaces are registered in the OCXContext as TopoDS_Shell.
     * @param vesselN the Vessel element
     * @return an assembly containing all reference surface
     */
    TopoDS_Shape ReadReferenceSurfaces( LDOM_Element & vesselN);

private:
    OCXContext * ctx;

};


#endif //OCXREADER_OCXREFERENCESURFACESREADER_H
