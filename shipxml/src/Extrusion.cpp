//
// Created by Zerbst on 01.12.2022.
//

#include "Extrusion.h"

shipxml::Extrusion::Extrusion() : EntityWithProperties(""){

}

double shipxml::Extrusion::Thickness() {
    return thickness;
}

void shipxml::Extrusion::Thickness(double t) {
    thickness = t;

}

double shipxml::Extrusion::Offset() {
    return offset;
}

void shipxml::Extrusion::Offset(double o) {
    offset = o;

}

shipxml::Orientation shipxml::Extrusion::Orientation() {
    return orientation;
}

void shipxml::Extrusion::Orientation(shipxml::Orientation o) {
    orientation = o;

}
