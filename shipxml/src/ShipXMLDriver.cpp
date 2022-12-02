//
// Created by cz on 01.12.22.
//

#include "../include/ShipXMLDriver.h"

#include <LDOM_XmlWriter.hxx>
#include <iostream>

#include "../include/ShipXMLHelper.h"
#include "../include/PanelReader.h"

namespace shipxml {

ShipXMLDriver::ShipXMLDriver() {}

bool ShipXMLDriver::Write( const std::string path) {
  auto writer = LDOM_XmlWriter();
  ofstream myfile;
  myfile.open (path);

  writer.SetIndentation(4);
  writer.Write(myfile, doc);
  return true;
}

bool ShipXMLDriver::Transfer(LDOM_Element  ode) {

  ocxDocEL=ode;

  // Create ShipSteelTransfer
  sst = ShipSteelTransfer();

  LDOM_Element vesselN = ShipXMLHelper::GetFirstChild(ocxDocEL, "Vessel");
  // and recursively walk down the OCX structure
  PanelReader( ).ReadPanels(vesselN, sst);


  // Create DOM
  doc = LDOM_Document::createDocument("abc");

  return true;
}
ShipSteelTransfer ShipXMLDriver::GetShipSteelTransfer() {
  return sst;
}

}  // namespace shipxml