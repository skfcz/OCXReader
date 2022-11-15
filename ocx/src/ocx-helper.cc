//
// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.
//

#include "ocx/internal/ocx-helper.h"

#include <string.h>

#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <memory>
#include <vector>

std::string OCXHelper::GetLocalTagName(const LDOM_Element &elem,
                                       bool keepPrefix) {
  auto tagName = std::string(elem.getTagName().GetString());
  if (size_t idx = tagName.find(':'); idx != std::string::npos && !keepPrefix) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

std::string OCXHelper::GetLocalAttrName(const LDOM_Node &elem) {
  auto tagName = std::string(elem.getNodeName().GetString());
  if (size_t idx = tagName.find(':'); idx != std::string::npos) {
    return tagName.substr(idx + 1);
  }
  return tagName;
}

std::string OCXHelper::GetChildTagName(const LDOM_Element &parent,
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

LDOM_Element OCXHelper::GetFirstChild(const LDOM_Element &parent,
                                      const std::string_view &localName) {
  // Verify preconditions
  LDOM_Element aVoidElement;
  if (parent == nullptr || localName.length() == 0) {
    return aVoidElement;
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

double OCXHelper::ReadDimension(const LDOM_Element &valueN,
                                const std::shared_ptr<OCXContext> &ctx) {
  double value = 0;
  OCXHelper::GetDoubleAttribute(valueN, "numericvalue", value);
  auto xUnit = std::string(valueN.getAttribute("unit").GetString());
  value *= ctx->LoopupFactor(xUnit);
  return value;
}

gp_Pnt OCXHelper::ReadPoint(const LDOM_Element &pointN,
                            const std::shared_ptr<OCXContext> &ctx) {
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

KnotMults OCXHelper::ParseKnotVector(std::string_view knotVectorS,
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

  // Parse the knot values under consideration of multiplicity
  std::vector<double> knots0;
  std::vector<int> mults0;
  // Get the quantity of knots and their multiplicities values
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
    if (abs(knotValue - lastKnot) < 1e-9) {
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

PolesWeightsCurve OCXHelper::ParseControlPointsCurve(
    LDOM_Element const &controlPtListN, int const &numCtrlPoints,
    std::string const &id, std::shared_ptr<OCXContext> const &ctx) {
  auto polesWeights = PolesWeightsCurve(numCtrlPoints);

  LDOM_Node childN = controlPtListN.getFirstChild();
  if (childN.isNull()) {
    std::cerr << "could not find NURBS3D/ControlPtList/ControlPoint[0] in "
                 "curve id='"
              << id << "'" << std::endl;
    polesWeights.IsNull = true;
    return polesWeights;
  }

  int i = 1;  // index starts at 1
  while (childN != nullptr) {
    // Check for valid node type
    const LDOM_Node::NodeType nodeType = childN.getNodeType();
    if (nodeType == LDOM_Node::ATTRIBUTE_NODE) {
      std::cerr
          << "expected ControlPoint node of type LDOM_Node::ELEMENT_NODE, "
             "but got LDOM_Node::ATTRIBUTE_NODE in curve id='"
          << id << "'" << std::endl;
      polesWeights.IsNull = true;
      return polesWeights;
    }

    // Parse the control point
    if (nodeType == LDOM_Node::ELEMENT_NODE) {
      LDOM_Element controlPointN = (LDOM_Element &)childN;
      LDOM_Element pointN = GetFirstChild(controlPointN, "Point3D");
      if (pointN.isNull()) {
        std::cerr << "could not find NURBS3D/ControlPtList/ControlPoint[" << i
                  << "] in curve id='" << id << "'" << std::endl;
        polesWeights.IsNull = true;
        return polesWeights;
      }

      // Read the point
      polesWeights.poles.SetValue(i, OCXHelper::ReadPoint(pointN, ctx));

      // Read the weight
      double weight = 1.0;  // default weight
      OCXHelper::GetDoubleAttribute(pointN, "weight", weight);
      polesWeights.weights.SetValue(i, weight);

      childN = controlPointN.getNextSibling();
      i++;
    }
  }

  return polesWeights;
}

PolesWeightsSurface OCXHelper::ParseControlPointsSurface(
    LDOM_Element const &controlPtListN, int const &uNumCtrlPoints,
    int const &vNumCtrlPoints, std::string const &id,
    std::shared_ptr<OCXContext> const &ctx) {
  auto polesWeights = PolesWeightsSurface(uNumCtrlPoints, vNumCtrlPoints);

  LDOM_Node childN = controlPtListN.getFirstChild();
  if (childN.isNull()) {
    std::cerr << "could not find NURBSSurface/ControlPtList/ControlPoint[0] in "
                 "surface id='"
              << id << "'" << std::endl;
    polesWeights.IsNull = true;
    return polesWeights;
  }

  for (int u = 1; u <= uNumCtrlPoints; u++) {
    for (int v = 1; v <= vNumCtrlPoints; v++) {
      // Check for valid node type
      const LDOM_Node::NodeType nodeType = childN.getNodeType();
      if (nodeType == LDOM_Node::ATTRIBUTE_NODE) {
        std::cerr
            << "expected ControlPoint node of type LDOM_Node::ELEMENT_NODE, "
               "but got LDOM_Node::ATTRIBUTE_NODE in surface id='"
            << id << "'" << std::endl;
        polesWeights.IsNull = true;
        return polesWeights;
      }

      // Parse the control point
      if (nodeType == LDOM_Node::ELEMENT_NODE) {
        LDOM_Element controlPointN = (LDOM_Element &)childN;
        LDOM_Element pointN = GetFirstChild(controlPointN, "Point3D");
        if (pointN.isNull()) {
          std::cerr << "could not find NURBSSurface/ControlPtList/ControlPoint["
                    << u << "," << v << "] in surface id='" << id << "'"
                    << std::endl;
          polesWeights.IsNull = true;
          return polesWeights;
        }

        // Read the point
        polesWeights.poles.SetValue(u, v, OCXHelper::ReadPoint(pointN, ctx));

        // Read the weight
        double weight = 1.0;  // default weight
        OCXHelper::GetDoubleAttribute(pointN, "weight", weight);
        polesWeights.weights.SetValue(u, v, weight);

        childN = controlPointN.getNextSibling();
      }
    }
  }

  return polesWeights;
}
