//
// Created by Zerbst on 01.12.2022.
//

#include "Structure.h"

namespace shipxml {
    list<Panel> Structure::Panels() {
        return panels;
    }

    list<Bracket> Structure::Brackets() {
        return brackets;
    }
} // shipxml