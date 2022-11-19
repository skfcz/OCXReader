// This file is part of OCXReader library
// Copyright Carsten Zerbst (carsten.zerbst@groy-groy.de)
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation.

#ifndef OCX_INCLUDE_OCX_OCX_CONTEXT_H_
#define OCX_INCLUDE_OCX_OCX_CONTEXT_H_

#include <LDOM_Element.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <map>
#include <string>

namespace ocx {

class OCXContext {
 public:
  OCXContext(const LDOM_Element &ocxDocN, const std::string &nsPrefix)
      : ocxDocN(ocxDocN),
        nsPrefix(nsPrefix),
        ocxGUIDRef(LDOMString((nsPrefix + ":GUIDRef").c_str())),
        ocxGUID(LDOMString((nsPrefix + ":GUID").c_str())) {}

  static inline bool CreateReferenceSurfaces = true;

  static inline bool CreatePanelContours = true;
  static inline bool CreatePanelSurfaces =
      true;  // depends on CreatePanelContours
  static inline bool CreatePlateSurfaces =
      true;  // depends on CreatePanelContours->CreatePanelSurfaces

  static inline bool CreateStiffenerTraces = true;

  [[nodiscard]] std::string Prefix() const;
  [[nodiscard]] LDOMString OCXGUIDRef() const;
  [[nodiscard]] LDOMString OCXGUID() const;

  void PrepareUnits();

  [[nodiscard]] double LoopupFactor(const std::string &unit) const;

  void RegisterSurface(TopoDS_Shape const &shell, std::string const &guid);

  [[nodiscard]] TopoDS_Shape LookupSurface(
      std::basic_string<char> const &guid) const;

  void OCAFDoc(const opencascade::handle<TDocStd_Document> &handle);
  [[nodiscard]] opencascade::handle<TDocStd_Document> OCAFDoc() const;
  [[nodiscard]] opencascade::handle<XCAFDoc_ShapeTool> OCAFShapeTool() const;
  [[nodiscard]] opencascade::handle<XCAFDoc_ColorTool> OCAFColorTool() const;

 private:
  std::map<std::string, TopoDS_Shape, std::less<>> guid2refPlane;
  std::map<std::string, double, std::less<>> unit2factor;

  LDOM_Element ocxDocN;

  std::string nsPrefix;
  LDOMString ocxGUIDRef;
  LDOMString ocxGUID;

  opencascade::handle<TDocStd_Document> ocafDoc;
  opencascade::handle<XCAFDoc_ShapeTool> ocafShapeTool;
  opencascade::handle<XCAFDoc_ColorTool> ocafColorTool;
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_CONTEXT_H_
