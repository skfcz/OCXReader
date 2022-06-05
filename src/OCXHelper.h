//
// This file is part of OCXReader library
// Copyright  Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#ifndef OCXREADER_OCXHELPER_H
#define OCXREADER_OCXHELPER_H

#include <string>
#include <LDOM_Element.hxx>
#include <vector>
#include <gp_Pnt.hxx>
#include "OCXContext.h"

class OCXHelper {

public:

   static  std::string GetLocalTagName(  LDOM_Element& elem);

   static LDOM_Element GetFirstChild(LDOM_Element& parent, std::string localName);

   static void TokenizeBySpace( std::string const & str, std::vector<std::string> &out);

   static void GetDoubleAttribute( LDOM_Element& elem, std::string attrName , Standard_Real & value);

    static gp_Pnt ReadPoint(LDOM_Element element, OCXContext *ctx);

    static gp_Dir ReadDirection(LDOM_Element dirN);

    static double ReadDimension(LDOM_Element valueN, OCXContext *ctx);
};

#endif //OCXREADER_OCXHELPER_H
