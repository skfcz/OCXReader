//
// Created by Zerbst on 01.12.2022.
//

#include "../include/ShipSteelTransfer.h"

namespace shipxml {
    ShipSteelTransfer::ShipSteelTransfer() {
      structure = new shipxml::Structure();

    }
const shipxml::Structure* ShipSteelTransfer::Structure() const {
  return this->structure;
}

} // shipxml