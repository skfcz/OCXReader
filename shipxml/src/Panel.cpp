//
// Created by Zerbst on 01.12.2022.
//

#include "Panel.h"

namespace shipxml {

    Panel::Panel(string n) : EntityWithProperties(n) {

    }

    Extrusion Panel::Extrusion() {
        return extrusion;
    }

    shipxml::Support Panel::Support() {
        return support;
    }

    list<shipxml::Limit> Panel::Limits() {
        return limits;
    }

    shipxml::AMCurve Panel::Geometry() {
        return geometry;
    }

    void Panel::Geometry(shipxml::AMCurve c) {
        geometry = c;

    }
} // shipxml