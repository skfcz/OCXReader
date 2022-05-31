//
// Created by cz on 26.05.22.
//
#include "OCXHelper.h"
#include <stdio.h>
#include <string.h>
#include <vector>


std::string OCXHelper::GetLocalTagName(  LDOM_Element &elem) {

    std::string tagName = std::string(elem.getTagName().GetString());
    int idx = tagName.find( ':');
    if ( idx != std::string::npos) {
        return tagName.substr( idx+1);
    }
    return tagName;


}

LDOM_Element OCXHelper::GetFirstChild(LDOM_Element &parent, std::string localName) {

    std::cout << "localName '" <<localName << "'" << std::endl;

    // Verify preconditions
    LDOM_Element aVoidElement;
    if ( parent == NULL ||  localName.length() ==0) {
        return aVoidElement;
    }

    // Take the first child. If it doesn't match look for other ones in a loop
    LDOM_Node aChildNode = parent.getFirstChild();
    while (aChildNode != NULL)
    {
        const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();

        if (aNodeType == LDOM_Node::ELEMENT_NODE)
        {
            LDOM_Element aNextElement = (LDOM_Element&) aChildNode;

            if ( localName.compare( GetLocalTagName( aNextElement))==0) {
                return aNextElement;
            }
        }
        aChildNode = aChildNode.getNextSibling();
    }
    return aVoidElement;

}

void OCXHelper::TokenizeBySpace(const std::string &str, std::vector<std::string> &out) {

    char * token = strtok( const_cast<char*>(str.c_str()), " ");
    while ( token != nullptr) {
        out.push_back( std::string(token));
        token = strtok( nullptr, " ");
    }

}

void OCXHelper::GetDoubleAttribute(LDOM_Element &elem, std::string attrName, Standard_Real & value) {

    LDOMString stringValue = elem.getAttribute( attrName.c_str());
    if ( strlen( stringValue.GetString()) > 0) {
      double d = strtod(stringValue.GetString(), NULL);
      value = Standard_Real( d);
      return;
    }

    int i;
    stringValue.GetInteger(i);
    value = Standard_Real ( (double) i);

}
