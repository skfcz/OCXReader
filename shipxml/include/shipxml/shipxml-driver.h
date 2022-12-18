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

#ifndef SHIPXML_INCLUDE_SHIPXML_SHIPXML_DRIVER_H_
#define SHIPXML_INCLUDE_SHIPXML_SHIPXML_DRIVER_H_

#include <LDOM_Document.hxx>
#include <LDOM_Element.hxx>
#include <string>

#include "ocx/ocx-context.h"
#include "shipxml/internal/shipxml-ship-steel-transfer.h"

namespace shipxml {

/**
 * The ShipXMLDriver is used to transfer an OCX document and
 * its parsed geometry into a ShipSteelTransfer and write that to disc.
 */
class ShipXMLDriver {
 public:
  ShipXMLDriver();
  ~ShipXMLDriver() = default;

  /**
   * Transfer the given OCX document and its parsed geometry into a
   * ShipSteelTransfer entity
   * @param ctx the OCX context holding the described information
   * @return true if the transfer was successfully
   */
  bool Transfer() const;

  /**
   * Write the ShipSteelTransfer into an XML file at the given path.
   * Requires previous successfully run of Transfer
   * @param filepath the filepath to write to
   * @return true if writing was successfully
   */
  bool Write(std::string const& filepath);

  /**
   * Access the ShipSteelTransfer created in the Transfer method
   * @return the ShipSteelTransfer object
   */
  [[nodiscard]] std::shared_ptr<shipxml::ShipSteelTransfer>
  GetShipSteelTransfer() const;

 private:
  std::shared_ptr<shipxml::ShipSteelTransfer> sst;

  LDOM_Document sxDoc;
  LDOM_Element sxRootEL;
  LDOM_Element sxStructureEL;

  void AddAttribute(LDOM_Element parentEL, std::string attrName,
                    std::string attrValue);
  void AddAttribute(LDOM_Element parentEL, std::string attrName,
                    bool attrValue);
  void WritePanels();
  void WriteProperties(EntityWithProperties ewp, LDOM_Element entityEL);
};

}  // namespace shipxml

#endif  // SHIPXML_INCLUDE_SHIPXML_SHIPXML_DRIVER_H_
