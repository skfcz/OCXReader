// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.



#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "../include/ShipXMLHelper.h"

namespace shipxml {

std::string ShipXMLHelper::GetLocalTagName(LDOM_Element const &elem,
                                       bool keepPrefix) {
  auto tagName = std::string(elem.getTagName().GetString());
  if (size_t idx = tagName.find(':'); idx != std::string::npos && !keepPrefix) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

std::string ShipXMLHelper::GetLocalAttrName(LDOM_Node const &elem) {
  auto tagName = std::string(elem.getNodeName().GetString());
  if (size_t idx = tagName.find(':'); idx != std::string::npos) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

std::string ShipXMLHelper::GetChildTagName(LDOM_Element const &parent,
                                       bool keepPrefix) {
  auto aChildNode = parent.getFirstChild();
  while (!aChildNode.isNull()) {
    if (const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        aNodeType == LDOM_Node::ELEMENT_NODE) {
      return GetLocalTagName((LDOM_Element &)aChildNode, keepPrefix);
    }
    aChildNode = aChildNode.getNextSibling();
  }
  return {};
}

LDOM_Element ShipXMLHelper::GetFirstChild(LDOM_Element const &parent,
                                      std::string_view localName) {
  // Verify preconditions
  if (parent == nullptr || localName.length() == 0) {
    return {};
  }

  // Take the first child. If it doesn't match look for other ones in a loop
  LDOM_Node aChildNode = parent.getFirstChild();
  while (aChildNode != nullptr) {
    if (const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
        aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aNextElement = (LDOM_Element &)aChildNode;

      if (localName == GetLocalTagName(aNextElement)) {
        return aNextElement;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  return {};
}

template <class ContainerT>
void ShipXMLHelper::Tokenize(std::string_view str, ContainerT &tokens,
                         std::string const &delimiters, bool trimEmpty) {
  std::string::size_type pos;
  std::string::size_type lastPos = 0;
  std::string::size_type length = str.length();

  using value_type = typename ContainerT::value_type;
  using size_type = typename ContainerT::size_type;

  while (lastPos < length + 1) {
    pos = str.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos) {
      pos = length;
    }

    if (pos != lastPos || !trimEmpty)
      tokens.push_back(
          value_type(str.data() + lastPos, (size_type)pos - lastPos));
    lastPos = pos + 1;
  }
}

void ShipXMLHelper::GetDoubleAttribute(LDOM_Element const &elem,
                                   std::string const &attrName,
                                   Standard_Real &value) {
  LDOMString stringValue = elem.getAttribute(attrName.c_str());
  if (stringValue.Type() == LDOMBasicString::StringType::LDOM_NULL) {
    return;
  }

  if (strlen(stringValue.GetString()) > 0) {
    value = Standard_Real(strtod(stringValue.GetString(), nullptr));
    return;
  }

  int i;
  stringValue.GetInteger(i);
  value = Standard_Real((double)i);
}

//double ShipXMLHelper::ReadDimension(LDOM_Element const &valueN,
//                                std::shared_ptr<OCXContext> const &ctx) {
//  double value = 0;
//  OCXHelper::GetDoubleAttribute(valueN, "numericvalue", value);
//  auto xUnit = std::string(valueN.getAttribute("unit").GetString());
//  value *= ctx->LoopupFactor(xUnit);
//  return value;
//}

//gp_Pnt ShipXMLHelper::ReadPoint(LDOM_Element const &pointN,
//                            std::shared_ptr<OCXContext> const &ctx) {
//  auto xT = LDOMString((ctx->Prefix() + ":X").c_str());
//  auto yT = LDOMString((ctx->Prefix() + ":Y").c_str());
//  auto zT = LDOMString((ctx->Prefix() + ":Z").c_str());
//
//  LDOM_Element xN = pointN.GetChildByTagName(xT);
//  LDOM_Element yN = pointN.GetChildByTagName(yT);
//  LDOM_Element zN = pointN.GetChildByTagName(zT);
//
//  double x = OCXHelper::ReadDimension(xN, ctx);
//  double y = OCXHelper::ReadDimension(yN, ctx);
//  double z = OCXHelper::ReadDimension(zN, ctx);
//
//  return {x, y, z};
//}

gp_Dir ShipXMLHelper::ReadDirection(const LDOM_Element &dirN) {
  double x = 0;
  ShipXMLHelper::GetDoubleAttribute(dirN, "x", x);
  double y = 0;
  ShipXMLHelper::GetDoubleAttribute(dirN, "y", y);
  double z = 0;
  ShipXMLHelper::GetDoubleAttribute(dirN, "z", z);

  return {x, y, z};
}
const char *ShipXMLHelper::ReadGUID(const LDOM_Element &element) {

  auto attributes = element.GetAttributesList();
  for( int i = 0; i < attributes.getLength();i++) {

      LDOM_Node n = attributes.item(i);
      auto attrName = GetLocalAttrName(n );
      //cout << "attr #" << i << " " <<attrName.c_str() << std::endl;
      if (strncmp("GUIDRef", attrName.c_str(),7 )==0) {
         return n.getNodeValue().GetString();
         break;
      }
  }
  return "";

}
std::string ShipXMLHelper::GetAttribute(LDOM_Element element, std::string attributeName) {
  auto attributes = element.GetAttributesList();
  for( int i = 0; i < attributes.getLength();i++) {

    LDOM_Node n = attributes.item(i);
    auto attrName = GetLocalAttrName(n );
    if (attributeName == attrName) {
      return n.getNodeValue().GetString();
      break;
    }
  }
  return "";
}

}  // namespace ocx
