/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXREFERENCESURFACESREADER_H
#define OCXREADER_OCXREFERENCESURFACESREADER_H

#include "OCXContext.h"

class OCXReferenceSurfacesReader {
   public:
    OCXReferenceSurfacesReader(OCXContext* ctx);

    /**
     * Read the references surfaces from the OCX file.
     * The references surfaces are registered in the OCXContext as TopoDS_Shell.
     * @param vesselN the Vessel element
     * @return an assembly containing all reference surface
     */
    TopoDS_Shape ReadReferenceSurfaces(LDOM_Element& vesselN);

   private:
    OCXContext* ctx;
};

#endif  // OCXREADER_OCXREFERENCESURFACESREADER_H
