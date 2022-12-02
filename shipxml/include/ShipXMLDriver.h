//
// Created by cz on 01.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_SHIPXMLDRIVER_H
#define OCXREADER_DISTRIBUTION_SHIPXMLDRIVER_H



#include <LDOM_Document.hxx>
#include <Standard_OStream.hxx>
#include <TopoDS.hxx>
#include <map>
#include <string>
#include "ShipSteelTransfer.h"



namespace shipxml {

class ShipXMLDriver {

 public:
  ShipXMLDriver();

  //bool Transfer( ocx::OCXContext c);

  bool Transfer(LDOM_Element ocxDoc);

  bool Write(  const std::string path  );
  ShipSteelTransfer GetShipSteelTransfer();


 private:
  LDOM_Element ocxDocEL;
  std::map<LDOM_Element, TopoDS_Shape> ocx2geometry;
  ShipSteelTransfer sst;
  LDOM_Document doc;

};

}  // namespace shipxml

#endif  // OCXREADER_DISTRIBUTION_SHIPXMLDRIVER_H
