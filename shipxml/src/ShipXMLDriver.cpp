//
// Created by cz on 01.12.22.
//

#include "../include/ShipXMLDriver.h"

#include <LDOM_XmlWriter.hxx>
#include <iostream>

#include "../include/CoordinateSystemReader.h"
#include "../include/PanelReader.h"
#include "../include/ShipXMLHelper.h"

namespace shipxml {

ShipXMLDriver::ShipXMLDriver() {
  sst = new ShipSteelTransfer();

}

bool ShipXMLDriver::Transfer(LDOM_Element ode, ocx::OCXContext * ctx) {

  ocxCtx = ctx;

  ocxDocEL=ode;
  ocxVesselEL = ShipXMLHelper::GetFirstChild(ocxDocEL, "Vessel");

  // Create ShipSteelTransfer

  LDOM_Element vesselN = ShipXMLHelper::GetFirstChild(ocxDocEL, "Vessel");

  CoordinateSystemReader(vesselN, sst, ocxCtx).ReadCoordinateSystem( );

  // and recursively walk down the OCX structure
  PanelReader( vesselN, sst, ocxCtx).ReadPanels();
  // TODO: Read Coordinate System, GeneralData and Catalogue





  return true;
}
ShipSteelTransfer *  ShipXMLDriver::GetShipSteelTransfer() {
  return sst;
}


bool ShipXMLDriver::Write( const std::string path) {

  // Create DOM
  sxDoc = LDOM_Document::createDocument("ShipSteelTransfer");
  sxRootEL = sxDoc.getDocumentElement();
  sxRootEL.appendChild(sxDoc.createComment("created by ShipXMLDriver") );

  // Set the transfer timestamp
  auto tsSX = sxDoc.createElement("timestamp");
  sxRootEL.appendChild(tsSX);
  char time_buf[21];
  time_t now;
  time(&now);
  strftime(time_buf, 21, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  tsSX.appendChild(sxDoc.createTextNode(time_buf));

  sxStructureEL = sxDoc.createElement("Structure");
  sxRootEL.appendChild( sxStructureEL);
  WritePanels();

  auto writer = LDOM_XmlWriter();
  ofstream myfile;
  myfile.open (path);

  writer.SetIndentation(4);
  writer.Write(myfile, sxDoc);
  return true;
}


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
//    sxPanelsEL.setAttribute( "categoryDes", it->CategoryDescription().c_str());
//    sxPanelsEL.setAttribute( "planar", it->Planar() ? "true" : "false");
//    sxPanelsEL.setAttribute( "pillar", it->Pillar()? "true":"false");
//    sxPanelsEL.setAttribute( "owner", it->Owner().c_str());
//    sxPanelsEL.setAttribute( "defaultMaterial", it->DefaultMaterial().c_str());
//    sxPanelsEL.setAttribute( "tightness", it->Tightness().c_str());
//    it++;
//  } while ( it != panels.end());

}


}  // namespace shipxml