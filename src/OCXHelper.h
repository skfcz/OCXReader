/*
 * This file is part of OCXReader library
 * Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 */

#ifndef OCXREADER_OCXHELPER_H
#define OCXREADER_OCXHELPER_H

#include <LDOM_Element.hxx>
#include <gp_Pnt.hxx>
#include <string>
#include <vector>

#include "OCXContext.h"

class OCXHelper {
   public:
    static std::string GetLocalTagName(LDOM_Element& elem);

    static std::string GetLocalAttrName(LDOM_Node& node);

    static LDOM_Element GetFirstChild(LDOM_Element& parent, std::string localName);

    static void TokenizeBySpace(std::string const& str, std::vector<std::string>& out);

    /**
     * Get the double attribute value from an attribute. Does not care if this is more
     * an integer '50' or double '50.0'. If the atttibute is missing the value is not touched
     * @param elem the element containing the attribute
     * @param attrName the attributes name
     * @param value the variable to put the value
     */
    static void GetDoubleAttribute(LDOM_Element& elem, std::string attrName, Standard_Real& value);

    static gp_Pnt ReadPoint(LDOM_Element element, OCXContext* ctx);

    static gp_Dir ReadDirection(LDOM_Element dirN);

    static double ReadDimension(LDOM_Element valueN, OCXContext* ctx);
};

#endif  // OCXREADER_OCXHELPER_H
