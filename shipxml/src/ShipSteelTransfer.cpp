//
// Created by Zerbst on 01.12.2022.
//

#include "../include/ShipSteelTransfer.h"

namespace shipxml {
    ShipSteelTransfer::ShipSteelTransfer() {

        this->structure = Structure();

    }

    Structure ShipSteelTransfer::Structure() {
        return this->structure;
    }
} // shipxml