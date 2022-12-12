//
// Created by Zerbst on 01.12.2022.
//

#include "../include/AMCurve.h"
#include "../include/enums.h"

shipxml::AMCurve::AMCurve(shipxml::AMSystem s) {
    system=s;
}

list<shipxml::ArcSegment> shipxml::AMCurve::Segments() {
    return segments;
}

shipxml::AMSystem shipxml::AMCurve::System() {
    return system;
}
