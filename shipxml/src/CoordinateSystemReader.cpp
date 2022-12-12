//
// Created by cz on 11.12.22.
//

#include "../include/CoordinateSystemReader.h"

namespace shipxml {
void CoordinateSystemReader::ReadCoordinateSystem() const {



}
CoordinateSystemReader::CoordinateSystemReader(const LDOM_Element& vesselN,
                                               ShipSteelTransfer * sstO,
                                               ocx::OCXContext* ctx) {
  ocxVesselEL = vesselN;
  sst= sstO;
  ocxContext = ctx;
}

}  // namespace shipxml