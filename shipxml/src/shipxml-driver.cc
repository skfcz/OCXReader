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
#include <iostream>
#include <memory>

#include "ocx/ocx-context.h"
#include "ocx/ocx-helper.h"
#include "shipxml/internal/shipxml-coordinate-system-reader.h"
#include "shipxml/internal/shipxml-exceptions.h"
#include "shipxml/internal/shipxml-log.h"
#include "shipxml/internal/shipxml-panel-reader.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"

namespace shipxml {

ShipXMLDriver::ShipXMLDriver() {
  if (ocx::OCXContext::GetInstance() == nullptr) {
    throw SHIPXMLNotFoundException("No OCX context available.");
  }
  sst = std::make_shared<shipxml::ShipSteelTransfer>();
}

//-----------------------------------------------------------------------------

bool ShipXMLDriver::Transfer() const {
  std::cout << "Transfer OCX/OCAD -> ShipXML" << std::endl;

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

  std::cout << "Write ShipXML -> XML" << std::endl;
  // Create DOM
  sxDoc = LDOM_Document::createDocument("ShipSteelTransfer");
  sxRootEL = sxDoc.getDocumentElement();
  sxRootEL.appendChild(sxDoc.createComment("created by ShipXMLDriver"));

  // Set the transfer timestamp
  auto tsSX = sxDoc.createElement("timestamp");
  sxRootEL.appendChild(tsSX);

  time_t now = time(0);
  struct tm tm = *gmtime(&now);

  char time_buf[21];
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
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
  auto sxPanelsEL = sxDoc.createElement("Panels");
  sxStructureEL.appendChild(sxPanelsEL);

   auto panels = sst->GetStructure()->GetPanels();

   for ( int i = 0; i < panels.size();i++) {

     Panel panel = panels.at(i);

      auto sxPanelEL = sxDoc.createElement("Panel");
      sxPanelsEL.appendChild(sxPanelEL);

      sxPanelEL.setAttribute( "name", panel.GetName().c_str());
      sxPanelEL.setAttribute( "blockName", panel.GetBlockName().c_str());
      sxPanelEL.setAttribute( "category", panel.GetCategory().c_str());
      sxPanelEL.setAttribute( "categoryDes", panel.GetCategoryDescription().c_str());
      sxPanelEL.setAttribute( "planar", panel.IsPlanar() ?  "true" : "false");
      sxPanelEL.setAttribute( "pillar", panel.IsPillar() ? "true":"false");
      sxPanelEL.setAttribute( "defaultMaterial", panel.GetDefaultMaterial().c_str());

      WriteProperties( panel, sxPanelEL);

      WriteSupport( panel, sxPanelEL);

      WriteGeometry( panel, sxPanelEL);

    }
}

void ShipXMLDriver::WriteProperties(EntityWithProperties ewp, LDOM_Element entityEL) {
  auto sxPropsEL = sxDoc.createElement("Properties");
  entityEL.appendChild(sxPropsEL);

  auto kvs = ewp.GetProperties().GetValues();
  for ( int i = 0; i < kvs.size();i++) {
    KeyValue kv  = kvs.at(i);
    auto sxKvEL = sxDoc.createElement("KeyValue");
    sxPropsEL.appendChild(sxKvEL);

    sxKvEL.setAttribute( "key", kv.GetKey().c_str());
    sxKvEL.setAttribute( "value", kv.GetValue().c_str());
    // TODO: support Unit

  }

}
void ShipXMLDriver::WriteSupport(Panel panel, LDOM_Element panelEL) {
  auto support = panel.GetSupport();

  auto sxSuppEL = sxDoc.createElement("Support");
  panelEL.appendChild(sxSuppEL);

  sxSuppEL.setAttribute("grid", support.GetGrid().c_str());
  sxSuppEL.setAttribute("coordinate", support.GetCoordinate().c_str());


  sxSuppEL.setAttribute("orientation", shipxml::ToString(support.GetOrientation()).c_str());
  sxSuppEL.setAttribute("planar", support.IsPlanar() ? "true" : "false");
  sxSuppEL.setAttribute("locationType", shipxml::ToString(support.GetLocationType()).c_str());
  sxSuppEL.setAttribute("majorPlane", shipxml::ToString(support.GetMajorPlane()).c_str());


 sxSuppEL.setAttribute("normal", shipxml::ToString(support.GetNormal()).c_str());
 sxSuppEL.setAttribute("tp1", shipxml::ToString(support.GetTP1()).c_str());
 sxSuppEL.setAttribute("tp2", shipxml::ToString(support.GetTP2()).c_str());
 sxSuppEL.setAttribute("tp3", shipxml::ToString(support.GetTP3()).c_str());

}

void ShipXMLDriver::WriteGeometry(Panel panel, LDOM_Element panelEL) {
  auto sxGeometryEL = sxDoc.createElement("Geometry");
  panelEL.appendChild(sxGeometryEL);

  auto crv = panel.GetGeometry();

  auto sxAMCrvEL = sxDoc.createElement("AMCurve");
  sxGeometryEL.appendChild(sxAMCrvEL);

  sxAMCrvEL.setAttribute("system", ToString(crv.GetSystem()).c_str());

  auto segments = crv.GetSegments();
  for (int i = 0; i < segments.size(); i++) {
    ArcSegment segment = segments.at(i);

    auto sxSegEL = sxDoc.createElement("ArcSegment");
    sxAMCrvEL.appendChild(sxSegEL);


    sxSegEL.setAttribute("isLine",segment.IsLine() ? "true" :"false");
    sxSegEL.setAttribute("startPoint", ToString(segment.GetStartPoint()).c_str());
    sxSegEL.setAttribute("endPoint", ToString(segment.GetEndPoint()).c_str());

    if ( ! segment.IsLine()) {
      sxSegEL.setAttribute("middlePoint", ToString(segment.GetPointOnCircle()).c_str());
    }
  }






}


}  // namespace shipxml
