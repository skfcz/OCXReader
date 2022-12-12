//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_SHIPSTEELTRANSFER_H
#define SHIPXML_SHIPSTEELTRANSFER_H

#include "Structure.h"

namespace shipxml {

    class ShipSteelTransfer {

    public :
        ShipSteelTransfer();
        const shipxml::Structure * Structure() const;

    private:
        shipxml::Structure  * structure;
    };

} // shipxml

#endif //SHIPXML_SHIPSTEELTRANSFER_H
