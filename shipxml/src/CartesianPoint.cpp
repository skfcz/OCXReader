//
// Created by Zerbst on 01.12.2022.
//


#include "../include/CartesianPoint.h"

namespace shipxml {
    CartesianPoint::CartesianPoint() {

    }

    CartesianPoint::CartesianPoint(double xi, double yi, double zi) {

        x= xi;
        y = yi;
        z=zi;

    }

    double CartesianPoint::X() {
        return x;
    }

    void CartesianPoint::X(double d) {
        x = d;
    }

    double CartesianPoint::Y() {
        return y;
    }

    void CartesianPoint::Y(double d) {
        y= d;
    }
    double CartesianPoint::Z() {
        return z;
    }

    void CartesianPoint::Z(double d) {
        z = d;
    }

} // shipxml