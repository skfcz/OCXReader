//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADER_OCXPANELREADER_H
#define OCXREADER_OCXPANELREADER_H


#include <TDF_Label.hxx>
#include "OCXContext.h"

class OCXPanelReader {
public:
    OCXPanelReader(OCXContext *ctx);

    TopoDS_Shape ParsePanels(LDOM_Element &vesselN);

private:
    OCXContext * ctx;

    TopoDS_Shape ParsePanel(LDOM_Element &pannelN);
};


#endif //OCXREADER_OCXPANELREADER_H
