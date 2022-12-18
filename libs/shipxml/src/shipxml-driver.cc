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
#include <memory>

#include "ocx/ocx-context.h"
#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-coordinate-system-reader.h"
#include "shipxml/internal/shipxml-exceptions.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel-reader.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"

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
  sst = std::make_shared<shipxml::ShipSteelTransfer>();
}

//-----------------------------------------------------------------------------

bool ShipXMLDriver::Transfer() const {
  LDOM_Element ocxDocEL = ocx::OCXContext::GetInstance()->OCXRoot();

  LDOM_Element vesselN = ocx::helper::GetFirstChild(ocxDocEL, "Vessel");

  CoordinateSystemReader(vesselN, sst).ReadCoordinateSystem();

  PanelReader panelReader(vesselN, sst);
  panelReader.ReadPanels();
  // TODO: Read Coordinate System, GeneralData and Catalogue

  return true;
}

//-----------------------------------------------------------------------------

bool ShipXMLDriver::Write(std::string const& filepath) {
  // Create DOM
  sxDoc = LDOM_Document::createDocument("ShipSteelTransfer");
  sxRootEL = sxDoc.getDocumentElement();
  sxRootEL.appendChild(sxDoc.createComment("created by ShipXMLDriver"));

  // Set the transfer timestamp
  auto tsSX = sxDoc.createElement("timestamp");
  sxRootEL.appendChild(tsSX);
  time_t t = time(nullptr);
  struct tm tstruct {};
  char time_buf[21];
  gmtime(&t, &tstruct);
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%SZ", &tstruct);
  tsSX.appendChild(sxDoc.createTextNode(time_buf));

  sxStructureEL = sxDoc.createElement("Structure");
  sxRootEL.appendChild(sxStructureEL);
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
  writer.Write(ofs, sxDoc);

  return true;
}

//-----------------------------------------------------------------------------

std::shared_ptr<shipxml::ShipSteelTransfer>
ShipXMLDriver::GetShipSteelTransfer() const {
  return sst;
}

//-----------------------------------------------------------------------------

void ShipXMLDriver::WritePanels() {
  //  auto sxPanelsEL = sxDoc.createElement("Panels");
  //  sxStructureEL.appendChild(sxPanelsEL);
  //
  //  auto panels = sst->Structure()->Panels();
  //
  //
  //  std::list<Panel*>::iterator it = panels.begin();
  //  do  {
  //
  //    auto sxPanelEL = sxDoc.createElement("Panel");
  //    sxPanelsEL.appendChild(sxPanelEL);
  //
  //    sxPanelsEL.setAttribute( "name", it->Name().c_str());
  //    sxPanelsEL.setAttribute( "blockName", it->BlockName().c_str());
  //    sxPanelsEL.setAttribute( "category", it->Category().c_str());
  //    sxPanelsEL.setAttribute( "categoryDes",
  //    it->CategoryDescription().c_str()); sxPanelsEL.setAttribute( "planar",
  //    it->Planar() ? "true" : "false"); sxPanelsEL.setAttribute( "pillar",
  //    it->Pillar()? "true":"false"); sxPanelsEL.setAttribute( "owner",
  //    it->Owner().c_str()); sxPanelsEL.setAttribute( "defaultMaterial",
  //    it->DefaultMaterial().c_str()); sxPanelsEL.setAttribute( "tightness",
  //    it->Tightness().c_str()); it++;
  //  } while ( it != panels.end());
}

}  // namespace shipxml
