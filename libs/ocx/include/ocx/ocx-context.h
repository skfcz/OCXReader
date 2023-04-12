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
#include "ocx/internal/ocx-principal-particulars-wrapper.h"
#include "ocx/internal/ocx-refplane-wrapper.h"
#include "ocx/internal/ocx-utils.h"
#include "ocx/internal/ocx-vessel-grid-wrapper.h"

namespace ocx {

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

  void RegisterHoleShape(std::string const &guid,
                         TopoDS_Shape const &holeShape);

  [[nodiscard]] TopoDS_Shape LookupHoleShape(std::string_view const &guid);

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
  void RegisterRefPlane(std::string const &guid,
                        ocx::context_entities::RefPlaneType const &type,
                        LDOM_Element const &element, gp_Dir const &normal,
                        gp_Pnt const &p0, gp_Pnt const &p1, gp_Pnt const &p2);

  /**
   * Get a previously registered X/Y/Z Refplane LDOM_Element by its GUID
   *
   * @param guid the guid
   * @return the LDOM_Element to lookup
   */
  [[nodiscard]] ocx::context_entities::RefPlaneWrapper LookupRefPlane(
      std::string_view const &guid);

  /**
   * Register a the ocx PrincipalParticulars
   *
   * @param PrincipalParticularsWrapper the PrincipalParticularsWrapper to
   * register
   */
  void RegisterPrincipalParticulars(
      ocx::context_entities::PrincipalParticularsWrapper const
          &PrincipalParticularsWrapper);

  /**
   * Get the ocx PrincipalParticular
   */
  [[nodiscard]] ocx::context_entities::PrincipalParticularsWrapper
  GetPrincipalParticulars();

  /**
   * Register a the ocx VesselGrid
   *
   * @param vesselGridWrappers the VesselGridWrappers to register
   */
  void RegisterVesselGrid(
      std::vector<ocx::context_entities::VesselGridWrapper> const
          &vesselGridWrappers);

  /**
   * Get the ocx VesselGrid
   */
  [[nodiscard]] std::vector<ocx::context_entities::VesselGridWrapper>
  GetVesselGrid();

  /**
   * Register a BarSection by its LDOM_Element (matched by given GUID or ID)
   *
   * @param shape the BarSection to register (one of TopoDS_Face or
   * TopoDS_Shell)
   * @param guid the GUID of the BarSection
   */
  void RegisterBarSection(LDOM_Element const &element,
                          ocx::context_entities::BarSection const &section);

  /**
   * Get a previously registered BarSection by its GUID.
   *
   * @param guid the GUID of the BarSection
   * @return the BarSection (one of TopoDS_Face or TopoDS_Shell) or empty
   * BarSection if not found
   */
  [[nodiscard]] ocx::context_entities::BarSection LookupBarSection(
      LDOM_Element const &element) const;

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

  /**
   * The ocx PrincipalParticular
   */
  ocx::context_entities::PrincipalParticularsWrapper m_principalParticulars;

  /**
   * The ocx VesselGrid
   */
  std::vector<ocx::context_entities::VesselGridWrapper> m_vesselGrid;

  std::map<LDOM_Element, TopoDS_Shape, LDOMCompare> LDOM2TopoDS_Shape;

  /**
   * Map of LDOM_Element to BarSection
   */
  std::map<LDOM_Element, ocx::context_entities::BarSection, LDOMCompare>
      LDOM2BarSection;

  std::map<std::string, TopoDS_Shape, std::less<>> m_holeCatalogue;

  /**
   * Map of GUID to RefPlaneWrapper
   */
  std::map<std::string, ocx::context_entities::RefPlaneWrapper, std::less<>>
      GUID2RefPlane;

  opencascade::handle<TDocStd_Document> ocafDoc;
  opencascade::handle<XCAFDoc_ShapeTool> ocafShapeTool;
  opencascade::handle<XCAFDoc_ColorTool> ocafColorTool;
};

}  // namespace ocx

#endif  // OCX_INCLUDE_OCX_OCX_CONTEXT_H_
