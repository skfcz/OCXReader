/***************************************************************************
 *   Created on: 03 Nov 2022                                               *
 ***************************************************************************
 *   Copyright (c) 2022, Carsten Zerbst (carsten.zerbst@groy-groy.de)      *
 *   Copyright (c) 2022, Paul Buechner                                     *
 *                                                                         *
 *   This file is part of the OCXReader library.                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   version 2.1 as published by the Free Software Foundation.             *
 *                                                                         *
 ***************************************************************************/

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
#include <utility>

#include "ocx-helper.h"
#include "ocx/internal/ocx-bar-section.h"
#include "ocx/internal/ocx-utils.h"

namespace ocx {

/**
 * RefPlaneType is used to identify the type of a reference plane.
 */
enum class RefPlaneType { X, Y, Z, UNDEF };

/**
 * RefPlaneWrapper is used to store the information of a reference plane.
 */
struct RefPlaneWrapper {
  std::string id;
  RefPlaneType type;
  LDOM_Element refPlaneN;
  gp_Dir normal;
  gp_Pnt p1;
  gp_Pnt p2;
  gp_Pnt p3;

  /**
   * Construct a default RefPlaneWrapper object with type UNDEF.
   */
  RefPlaneWrapper();

  /**
   * Construct a RefPlaneWrapper object from given parameters.
   *
   * @param type the type of the reference plane
   * @param refPlaneN the reference plane element
   * @param normal the normal of the reference plane
   * @param p1 the first point of the reference plane
   * @param p2 the second point of the reference plane
   * @param p3 the third point of the reference plane
   */
  RefPlaneWrapper(std::string const &id, RefPlaneType const &type, LDOM_Element const &refPlaneN,
                  gp_Dir const &normal, gp_Pnt const &p1, gp_Pnt const &p2,
                  gp_Pnt const &p3);
};

/**
 * Comparator to handle LDOM_Element as key in std::map
 */
struct LDOMCompare {
  bool operator()(LDOM_Element const &lhs, LDOM_Element const &rhs) const;
};

class OCXContext {
 public:
  OCXContext(OCXContext const &) = delete;
  OCXContext &operator=(OCXContext const &) = delete;

  static void Initialize(LDOM_Element const &root, std::string const &nsPrefix);

  static std::shared_ptr<OCXContext> GetInstance();

  static inline bool CreateReferenceSurfaces = true;

  static inline bool CreateLimitedBy = true;
  static inline bool CreatePanelContours = true;
  static inline bool CreateCutBy = true;
  static inline bool CreatePanelSurfaces = true;

  static inline bool CreateComposedOf = true;
  static inline bool CreatePlateContours = true;
  static inline bool CreatePlateSurfaces = true;

  static inline bool CreateStiffenerTraces = true;

  static inline double MinX = -10;
  static inline double MaxX = 190;
  static inline double MinY = -50;
  static inline double MaxY = 50;
  static inline double MinZ = -10;
  static inline double MaxZ = 45;

  /**
   * Get the OCX document root element
   *
   * @return the document root element
   */
  [[nodiscard]] LDOM_Element OCXRoot() const;

  /**
   * The document tag prefix
   *
   * @return the document tag prefix
   */
  [[nodiscard]] std::string Prefix() const;

  /**
   * @brief Setup units defined by the OCX document. Fallback to a set of
   * default units if no units have been found.
   */
  void PrepareUnits();

  /**
   * Get the loopup-factor for a given unit.
   *
   * @param unit the unit to lookup (one of Um, Udm, Ucm, Umm)
   * @return the factor to convert the given unit to the main unit defined by
   * the OCX document
   */
  [[nodiscard]] double LoopupFactor(const std::string &unit) const;

  /**
   * @brief Register a TopoDS_Shape by its LDOM_Element (matched by given
   * GUID or ID)
   *
   * @param element the LDOM_Element
   * @param shape the TopoDS_Shape
   */
  void RegisterShape(LDOM_Element const &element, TopoDS_Shape const &shape);

  /**
   * @brief Get a previously registered Shape by its LDOM_Element (matched by
   * given GUID or ID)
   *
   * @param element the LDOM_Element to lookup
   * @return the shape if found, otherwise an empty shape
   */
  [[nodiscard]] TopoDS_Shape LookupShape(LDOM_Element const &element);

  /**
   * Register an X/Y/ZRefPlane element by its GUID
   *
   * @param guid
   * @param type
   * @param element
   * @param normal
   * @param p0
   * @param p1
   * @param p2
   */
  void RegisterRefPlane(std::string const &guid, RefPlaneType const &type,
                        LDOM_Element const &element, gp_Dir const &normal,
                        gp_Pnt const &p0, gp_Pnt const &p1, gp_Pnt const &p2);

  /**
   * @brief Get a previously registered X/Y/Z Refplane LDOM_Element by its GUID
   *
   * @param guid the guid
   * @return the LDOM_Element to lookup
   */
  [[nodiscard]] RefPlaneWrapper LookupRefPlane(std::string_view const &guid);

  /**
   * Register a BarSection by its LDOM_Element (matched by given GUID or ID)
   *
   * @param shape the BarSection to register (one of TopoDS_Face or
   * TopoDS_Shell)
   * @param guid the GUID of the BarSection
   */
  void RegisterBarSection(LDOM_Element const &element,
                          BarSection const &section);

  /**
   * Get a previously registered BarSection by its GUID.
   *
   * @param guid the GUID of the BarSection
   * @return the BarSection (one of TopoDS_Face or TopoDS_Shell) or empty
   * BarSection if not found
   */
  [[nodiscard]] BarSection LookupBarSection(LDOM_Element const &element) const;

  void OCAFDoc(const opencascade::handle<TDocStd_Document> &handle);
  [[nodiscard]] opencascade::handle<TDocStd_Document> OCAFDoc() const;
  [[nodiscard]] opencascade::handle<XCAFDoc_ShapeTool> OCAFShapeTool() const;
  [[nodiscard]] opencascade::handle<XCAFDoc_ColorTool> OCAFColorTool() const;

 private:
  SHARED_PTR_CREATE(OCXContext);
  OCXContext(LDOM_Element const &root, std::string nsPrefix);

  /**
   * The OCXContext instance
   */
  static std::shared_ptr<OCXContext> s_instance;

  /**
   * The document root element
   */
  LDOM_Element m_root;

  /**
   * The document tag prefix
   */
  std::string m_nsPrefix;

  std::map<std::string, double, std::less<>> unit2factor;

  std::map<LDOM_Element, TopoDS_Shape, LDOMCompare> LDOM2TopoDS_Shape;
  std::map<LDOM_Element, BarSection, LDOMCompare> LDOM2BarSection;
  std::map<std::string, RefPlaneWrapper, std::less<>> GUID2RefPlane;

  opencascade::handle<TDocStd_Document> ocafDoc;
  opencascade::handle<XCAFDoc_ShapeTool> ocafShapeTool;
  opencascade::handle<XCAFDoc_ColorTool> ocafColorTool;
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_CONTEXT_H_
