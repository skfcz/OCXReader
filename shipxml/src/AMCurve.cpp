//
// Created by Zerbst on 01.12.2022.
//

#include "AMCurve.h"

shipxml::AMCurve::AMCurve(shipxml::AMSystem s) {
    system=s;
}

list<shipxml::ArcSegment> shipxml::AMCurve::Segments() {
    return segments;
}

shipxml::AMSystem shipxml::AMCurve::System() {
    return system;
}
