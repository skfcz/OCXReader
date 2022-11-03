//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-helper.h"

#include <stdio.h>
#include <string.h>

#include <vector>

std::string OCXHelper::GetLocalTagName(LDOM_Element &elem) {
  std::string tagName = std::string(elem.getTagName().GetString());
  int idx = tagName.find(':');
  if (idx != std::string::npos) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

std::string OCXHelper::GetLocalAttrName(LDOM_Node &elem) {
  std::string tagName = std::string(elem.getNodeName().GetString());
  int idx = tagName.find(':');
  if (idx != std::string::npos) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

LDOM_Element OCXHelper::GetFirstChild(LDOM_Element &parent,
                                      std::string localName) {
  // std::cout << "localName '" <<localName << "'" << std::endl;

  // Verify preconditions
  LDOM_Element aVoidElement;
  if (parent == NULL || localName.length() == 0) {
    return aVoidElement;
  }

  // Take the first child. If it doesn't match look for other ones in a loop
  LDOM_Node aChildNode = parent.getFirstChild();
  while (aChildNode != NULL) {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();

    if (aNodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element aNextElement = (LDOM_Element &)aChildNode;

      if (localName.compare(GetLocalTagName(aNextElement)) == 0) {
        return aNextElement;
      }
    }
    aChildNode = aChildNode.getNextSibling();
  }
  return aVoidElement;
}

template <class ContainerT>
void OCXHelper::Tokenize(std::string_view str, ContainerT &tokens,
                         const std::string &delimiters, bool trimEmpty) {
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

void OCXHelper::GetDoubleAttribute(const LDOM_Element &elem,
                                   const std::string &attrName,
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

double OCXHelper::ReadDimension(const LDOM_Element &valueN, OCXContext *ctx) {
  double value = 0;
  OCXHelper::GetDoubleAttribute(valueN, "numericvalue", value);
  auto xUnit = std::string(valueN.getAttribute("unit").GetString());
  value *= ctx->LoopupFactor(xUnit);
  return value;
}

gp_Pnt OCXHelper::ReadPoint(const LDOM_Element &pointN, OCXContext *ctx) {
  auto xT = LDOMString((ctx->Prefix() + ":X").c_str());
  auto yT = LDOMString((ctx->Prefix() + ":Y").c_str());
  auto zT = LDOMString((ctx->Prefix() + ":Z").c_str());

  LDOM_Element xN = pointN.GetChildByTagName(xT);
  LDOM_Element yN = pointN.GetChildByTagName(yT);
  LDOM_Element zN = pointN.GetChildByTagName(zT);

  double x = OCXHelper::ReadDimension(xN, ctx);
  double y = OCXHelper::ReadDimension(yN, ctx);
  double z = OCXHelper::ReadDimension(zN, ctx);

  return {x, y, z};
}

gp_Dir OCXHelper::ReadDirection(const LDOM_Element &dirN) {
  double x = 0;
  OCXHelper::GetDoubleAttribute(dirN, "x", x);
  double y = 0;
  OCXHelper::GetDoubleAttribute(dirN, "y", y);
  double z = 0;
  OCXHelper::GetDoubleAttribute(dirN, "z", z);

  return {x, y, z};
}

KnotMults OCXHelper::ParseKnotVector(const std::string &knotVectorS,
                                     int numKnots) {
  auto kn = KnotMults();

  std::vector<std::string> out;
  OCXHelper::Tokenize(knotVectorS, out);

  if (numKnots != static_cast<int>(out.size())) {
    std::cerr << "failed to parse knotvector, expected #" << numKnots
              << " knot values, but parsing [" << knotVectorS
              << "] resulted in #" << out.size() << " values" << std::endl;
    kn.IsNull = true;
    return kn;
  }

  // Parse the knot values under consideration of multiplicity.
  std::vector<double> knots0;
  std::vector<int> mults0;
  // Get the quantity of knots and their multiplicities values..
  double lastKnot;
  int mult;
  for (int i = 0; i < out.size(); i++) {
    double knotValue = std::stod(out[i]);
    // First knot value
    if (i == 0) {
      lastKnot = knotValue;
      mult = 1;
      continue;
    }
    // Increase multiplicity if knot value is nearly the same as the last one
    if (abs(knotValue - lastKnot) < 1e-3) {
      mult++;
      // Don't forget to push, if it's the last knot value
      if (i == out.size() - 1) {
        knots0.push_back(knotValue);
        mults0.push_back(mult);
      }
    } else {
      knots0.push_back(lastKnot);
      mults0.push_back(mult);
      lastKnot = knotValue;
      mult = 1;
    }
  }

  // Resize the knot and multiplicity arrays to the needed size
  kn.knots.Resize(1, (int)knots0.size(), false);
  kn.mults.Resize(1, (int)mults0.size(), false);

  // Set the knot and multiplicity values
  for (int i = 0; i < knots0.size(); i++) {
    kn.knots.SetValue(i + 1, knots0[i]);
    kn.mults.SetValue(i + 1, mults0[i]);
  }

  return kn;
}

PolesWeights OCXHelper::ParseControlPoints(const LDOM_Element &controlPtListN,
                                           int uNumCtrlPoints,
                                           int vNumCtrlPoints,
                                           const std::string &id,
                                           OCXContext *ctx) {
  auto polesWeights = PolesWeights();

  TColgp_Array2OfPnt cpArray(1, uNumCtrlPoints, 1, vNumCtrlPoints);
  TColStd_Array1OfInteger uMultsArray(1, uNumCtrlPoints);
  TColStd_Array1OfInteger vMultsArray(1, vNumCtrlPoints);

  LDOM_Element controlPointN =
      controlPtListN.GetChildByTagName(controlPtListN.getTagName());
  if (controlPointN.isNull()) {
    std::cerr << "could not find NURBSSurface/ControlPtList/ControlPoint[0] in "
                 "surface id='"
              << id << "'" << std::endl;
    polesWeights.IsNull = true;
    return polesWeights;
  }

  int i = 0;
  int u = 1;
  int v = 1;
  while (!controlPointN.isNull()) {
    double weight = 1;
    OCXHelper::GetDoubleAttribute(controlPointN, "weight", weight);

    LDOM_Element pointN = GetFirstChild(controlPointN, "Point3D");
    if (pointN.isNull()) {
      std::cerr << "could not find NURBSSurface/ControlPtList/ControlPoint["
                << i << "]/Point3D in surface id='" << id << "'" << std::endl;
      polesWeights.IsNull = true;
      return polesWeights;
    }
    gp_Pnt point = ReadPoint(pointN, ctx);
    LDOM_Element xN = pointN.GetChildByTagName("xT");
    LDOM_Element yN = pointN.GetChildByTagName("yT");
    LDOM_Element zN = pointN.GetChildByTagName("zT");

    double x = 0;
    OCXHelper::GetDoubleAttribute(xN, "numericvalue", x);
    auto xUnit = std::string(xN.getAttribute("unit").GetString());
    x *= ctx->LoopupFactor(xUnit);

    double y = 0;
    OCXHelper::GetDoubleAttribute(yN, "numericvalue", y);
    auto yUnit = std::string(yN.getAttribute("unit").GetString());
    y *= ctx->LoopupFactor(yUnit);

    double z = 0;
    OCXHelper::GetDoubleAttribute(zN, "numericvalue", z);
    auto zUnit = std::string(zN.getAttribute("unit").GetString());
    z *= ctx->LoopupFactor(zUnit);

    cpArray.SetValue(u, v, point);

    std::cout << controlPointN.getTagName().GetString() << " #" << i
              << " weight " << weight << ", [" << x << ", " << y << "," << z
              << "] " << xUnit << std::endl;

    // polesWeights.poles.SetValue(i + 1, gp_Pnt(x, y, z));
    // polesWeights.weights.SetValue(i + 1, weight);

    controlPointN = controlPointN.GetSiblingByTagName();
    i++;
  }

  // for (int i = 1; i <= uNumCtrlPoints; i++) {
  //   for (int j = 1; j <= vNumCtrlPoints; j++) {
  //
  //   }
  // }
  return polesWeights;
}
