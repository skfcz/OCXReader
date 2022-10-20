/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXPANELREADER_H
#define OCXREADER_OCXPANELREADER_H

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <list>

#include "OCXCurveReader.h"
#include "OCXHelper.h"
#include "OCXPanelReader.h"
#include "OCXSurfaceReader.h"

class OCXPanelReader {
   public:
    OCXPanelReader(OCXContext *ctx);

    TopoDS_Shape ReadPanels(LDOM_Element &vesselN);

   private:
    OCXContext *ctx;

    TopoDS_Shape ReadPanel(LDOM_Element &panelN);

    TopoDS_Wire ReadPanelOuterContour(LDOM_Element &panelN);
    TopoDS_Shape ReadPanelSurface(LDOM_Element &panelN, TopoDS_Wire &outerContour);

    TopoDS_Shape ReadPlates(LDOM_Element &panelN, TopoDS_Shape &referenceSurface);
    TopoDS_Shape ReadPlate(LDOM_Element &plateN, TopoDS_Shape &referenceSurface);

    TopoDS_Shape ReadStiffeners(LDOM_Element &panelN);
    TopoDS_Shape ReadStiffener(LDOM_Element &stiffenerN);

    TopoDS_Shape ReadBrackets(LDOM_Element &panelN);
    TopoDS_Shape ReadBracket(LDOM_Element &bracketN);
};

#endif  // OCXREADER_OCXPANELREADER_H
