//
// Created by cz on 26.05.22.
//

#ifndef OCXREADER_OCXHELPER_H
#define OCXREADER_OCXHELPER_H

#include <string>
#include <LDOM_Element.hxx>
#include <vector>

class OCXHelper {

public:

   static  std::string GetLocalTagName(  LDOM_Element& elem);

   static LDOM_Element GetFirstChild(LDOM_Element& parent, std::string localName);

   static void TokenizeBySpace( std::string const & str, std::vector<std::string> &out);

   static void GetDoubleAttribute( LDOM_Element& elem, std::string attrName , Standard_Real & value);

};

#endif //OCXREADER_OCXHELPER_H
