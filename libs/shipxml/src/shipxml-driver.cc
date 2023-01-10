/***************************************************************************
 *   Created on: 01 Dec 2022                                               *
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

#include "shipxml/shipxml-driver.h"

#include <LDOM_XmlWriter.hxx>
#include <ctime>
#include <magic_enum.hpp>
#include <memory>

#include "ocx/ocx-context.h"
#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-am-curve.h"
#include "shipxml/internal/shipxml-coordinate-system-reader.h"
#include "shipxml/internal/shipxml-entity-with-properties.h"
#include "shipxml/internal/shipxml-exceptions.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel-reader.h"
#include "shipxml/internal/shipxml-panel.h"
#include "shipxml/internal/shipxml-plate.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"
#include "shipxml/internal/shipxml-support.h"

#ifdef _MSC_VER
// Microsoft Visual C++ compiler
#define gmtime(t, tstruct) gmtime_s(tstruct, t)
#elif defined(__STDC_WANT_LIB_EXT1__)
// Other compiler with support for the __STDC_WANT_LIB_EXT1__ macro
#define gmtime(t, tstruct) gmtime_s(tstruct, t)
#else
// Other compiler without support for the __STDC_WANT_LIB_EXT1__ macro
#define gmtime(t, tstruct) gmtime_r(t, tstruct)
#endif

namespace shipxml {

ShipXMLDriver::ShipXMLDriver() {
  if (ocx::OCXContext::GetInstance() == nullptr) {
    throw SHIPXMLNotFoundException("No OCX context available.");
  }
  m_sst = std::make_shared<shipxml::ShipSteelTransfer>();
}

//-----------------------------------------------------------------------------

bool ShipXMLDriver::Transfer() const {
  LDOM_Element ocxDocEL = ocx::OCXContext::GetInstance()->OCXRoot();

  LDOM_Element vesselN = ocx::helper::GetFirstChild(ocxDocEL, "Vessel");

  CoordinateSystemReader(vesselN, m_sst).ReadCoordinateSystem();

  PanelReader panelReader(vesselN, m_sst);
  panelReader.ReadPanels();
  // TODO: Read Coordinate System, GeneralData and Catalogue

  return true;
}

//-----------------------------------------------------------------------------

bool ShipXMLDriver::Write(std::string const &filepath) {
  // Create DOM
  m_sxDoc = LDOM_Document::createDocument("ShipSteelTransfer");
  m_sxRootEL = m_sxDoc.getDocumentElement();
  m_sxRootEL.appendChild(m_sxDoc.createComment("created by ShipXMLDriver"));

  // Set the transfer timestamp
  auto tsSX = m_sxDoc.createElement("timestamp");
  m_sxRootEL.appendChild(tsSX);
  time_t t = time(nullptr);
  struct tm tstruct {};
  char time_buf[21];
  gmtime(&t, &tstruct);
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%SZ", &tstruct);
  tsSX.appendChild(m_sxDoc.createTextNode(time_buf));

  m_sxStructureEL = m_sxDoc.createElement("Structure");
  m_sxRootEL.appendChild(m_sxStructureEL);
  WritePanels();

  // Configure XML file writer
  auto writer = LDOM_XmlWriter();
  writer.SetIndentation(4);

  // Write to file
  std::ofstream ofs(filepath);
  if (!ofs) {
    SHIPXML_ERROR("Could not open file {} for writing.", filepath)
    return false;
  }
  writer.Write(ofs, m_sxDoc);

  return true;
}

//-----------------------------------------------------------------------------

std::shared_ptr<shipxml::ShipSteelTransfer>
ShipXMLDriver::GetShipSteelTransfer() const {
  return m_sst;
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WritePanels() {
  auto sxPanelsEL = m_sxDoc.createElement("Panels");
  m_sxStructureEL.appendChild(sxPanelsEL);

  auto panels = m_sst->GetStructure()->GetPanels();

  for (Panel const &panel : panels) {
    auto sxPanelEL = m_sxDoc.createElement("Panel");
    sxPanelsEL.appendChild(sxPanelEL);

    sxPanelEL.setAttribute("name", panel.GetName().c_str());
    sxPanelEL.setAttribute("blockName", panel.GetBlockName().c_str());
    sxPanelEL.setAttribute("category", panel.GetCategory().c_str());
    sxPanelEL.setAttribute("categoryDes",
                           panel.GetCategoryDescription().c_str());
    sxPanelEL.setAttribute("planar", panel.IsPlanar() ? "true" : "false");
    sxPanelEL.setAttribute("pillar", panel.IsPillar() ? "true" : "false");
    sxPanelEL.setAttribute("defaultMaterial",
                           panel.GetDefaultMaterial().c_str());

    WriteProperties((EntityWithProperties &)panel, sxPanelEL);

    WriteSupport(panel.GetSupport(), sxPanelEL);

    WriteGeometry(panel.GetGeometry(), sxPanelEL);

    WritePlates(panel.GetPlates(), sxPanelEL);
  }
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WriteProperties(EntityWithProperties &ewp,
                                    LDOM_Element &entityEL) {
  auto sxPropsEL = m_sxDoc.createElement("Properties");
  entityEL.appendChild(sxPropsEL);

  auto props = ewp.GetProperties().GetValues();
  for (auto const &prop : props) {
    auto sxPropEL = m_sxDoc.createElement("KeyValue");
    sxPropsEL.appendChild(sxPropEL);

    sxPropEL.setAttribute("key", prop.GetKey().c_str());
    sxPropEL.setAttribute("value", prop.GetValue().c_str());
    // TODO: support Unit
    // sxPropEL.setAttribute("unit", prop.GetUnit().c_str());
  }
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WriteSupport(Support const &support,
                                 LDOM_Element &panelEL) {
  auto sxSuppEL = m_sxDoc.createElement("Support");
  panelEL.appendChild(sxSuppEL);

  sxSuppEL.setAttribute("grid", support.GetGrid().c_str());
  sxSuppEL.setAttribute("coordinate", support.GetCoordinate().c_str());

  sxSuppEL.setAttribute("orientation",
                        magic_enum::enum_name(support.GetOrientation()).data());
  sxSuppEL.setAttribute("planar", support.IsPlanar() ? "true" : "false");
  sxSuppEL.setAttribute(
      "locationType", magic_enum::enum_name(support.GetLocationType()).data());
  sxSuppEL.setAttribute("majorPlane",
                        magic_enum::enum_name(support.GetMajorPlane()).data());

  sxSuppEL.setAttribute("normal", support.GetNormal().ToString().c_str());
  sxSuppEL.setAttribute("tp1", support.GetTP1().ToString().c_str());
  sxSuppEL.setAttribute("tp2", support.GetTP2().ToString().c_str());
  sxSuppEL.setAttribute("tp3", support.GetTP3().ToString().c_str());
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WriteGeometry(AMCurve const &crv, LDOM_Element &panelEL) {
  auto sxGeometryEL = m_sxDoc.createElement("Geometry");
  panelEL.appendChild(sxGeometryEL);

  auto sxAMCrvEL = m_sxDoc.createElement("AMCurve");
  sxGeometryEL.appendChild(sxAMCrvEL);

  sxAMCrvEL.setAttribute("system",
                         magic_enum::enum_name(crv.GetSystem()).data());

  auto segments = crv.GetSegments();
  for (ArcSegment const &seg : segments) {
    auto sxSegEL = m_sxDoc.createElement("ArcSegment");
    sxAMCrvEL.appendChild(sxSegEL);

    sxSegEL.setAttribute("isLine", seg.IsLine() ? "true" : "false");
    sxSegEL.setAttribute("startPoint", seg.GetStartPoint().ToString().c_str());
    sxSegEL.setAttribute("endPoint", seg.GetEndPoint().ToString().c_str());

    if (!seg.IsLine()) {
      sxSegEL.setAttribute("middlePoint",
                           seg.GetPointOnCircle().ToString().c_str());
    }
  }
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WritePlates(std::vector<Plate> const &plates,
                                LDOM_Element &panelEL) {
  if (plates.empty()) {
    return;
  }

  auto sxPlatesEL = m_sxDoc.createElement("Plates");
  panelEL.appendChild(sxPlatesEL);

  for (Plate const &plate : plates) {
    auto sxPlateEL = m_sxDoc.createElement("Plate");
    sxPlatesEL.appendChild(sxPlateEL);

    sxPlateEL.setAttribute("name", plate.GetName().c_str());
    sxPlateEL.setAttribute("category", plate.GetCategory().c_str());
    sxPlateEL.setAttribute("categoryDes",
                           plate.GetCategoryDescription().c_str());
    sxPlateEL.setAttribute("material", plate.GetMaterial().c_str());
    sxPlateEL.setAttribute("material", plate.GetMaterial().c_str());
    sxPlateEL.setAttribute("thickness",
                           std::to_string(plate.GetThickness()).c_str());
    sxPlateEL.setAttribute("offset", std::to_string(plate.GetOffset()).c_str());
    sxPlateEL.setAttribute(
        "orientation", magic_enum::enum_name(plate.GetOrientation()).data());

    WriteProperties((EntityWithProperties &)plate, sxPlateEL);

    WriteGeometry(plate.GetGeometry(), sxPlateEL);
  }
}

}  // namespace shipxml
