//
// Created by cz on 11.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_COORDINATESYSTEMREADER_H
#define OCXREADER_DISTRIBUTION_COORDINATESYSTEMREADER_H

namespace shipxml {

class CoordinateSystemReader {
 public:
  CoordinateSystemReader( LDOM_Element const &vesselN, ShipSteelTransfer * sst, ocx::OCXContext * ctx);
  void ReadCoordinateSystem() const;

 private:

  ShipSteelTransfer * sst;
  ocx::OCXContext * ocxContext;
  LDOM_Element ocxVesselEL;



};

}  // namespace shipxml

#endif  // OCXREADER_DISTRIBUTION_COORDINATESYSTEMREADER_H
