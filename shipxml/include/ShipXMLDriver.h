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

/**
 * The ShipXMLDriver is used to transfer an OCX document +
 * precalculated geometry into a ShipSteelTransfer and write that to disc.
 */
class ShipXMLDriver {

 public:
  ShipXMLDriver();

  //bool Transfer( ocx::OCXContext c);


  /**
   * Transfer the given OCX document + precalculated geometry into a
   * ShipSteelTransfer entity
   * @param ocxDoc the root node of the OCX document
   * @return true if the transfer was successfully
   */
  bool Transfer(LDOM_Element ocxDoc);

  /**
   * Write the ShipSteelTransfer into an XML file at the given path.
   * Requires previous successfull run of Transfer
   * @param path the file to write to
   * @return true if writing was successfull
   */
  bool Write(  const std::string path  );

  /**
   * Access the ShipSteelTransfer created in the Transfer method
   * @return  the ShipSteelTransfer object
   */
  ShipSteelTransfer GetShipSteelTransfer();

 private:

  std::map<LDOM_Element, TopoDS_Shape> ocx2geometry;

  LDOM_Element ocxDocEL;
  LDOM_Element ocxVesselEL;

  ShipSteelTransfer sst;
  LDOM_Document sxDoc;
  LDOM_Element sxRootEL;
  LDOM_Element sxStructureEL;


  void AddAttribute( LDOM_Element parentEL, std::string attrName, std::string attrValue);
  void AddAttribute( LDOM_Element parentEL, std::string attrName, bool attrValue);
  void WritePanels( );



};

}  // namespace shipxml

#endif  // OCXREADER_DISTRIBUTION_SHIPXMLDRIVER_H
