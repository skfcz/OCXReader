//
// Created by Zerbst on 01.12.2022.
//

#include "../include/Limit.h"

shipxml::Limit::Limit(string n) : shipxml::NamedEntity(n){

}

shipxml::AMCurve shipxml::Limit::Contour() {
    return contour;
}

void shipxml::Limit::Contour(shipxml::AMCurve c) {
    contour = c;
}

string shipxml::Limit::Feature() {
    return feature;
}

void shipxml::Limit::Feature(string s) {
    feature = s;

}

shipxml::LimitType shipxml::Limit::LimitType() {

    return limitType;
}

double shipxml::Limit::Offset() {
    return offset;
}

void shipxml::Limit::Offset(double o) {
offset=o;
}

shipxml::CartesianPoint shipxml::Limit::P0() {
    return p0;
}

void shipxml::Limit::P0(shipxml::CartesianPoint p) {
    p0 = p;
}

shipxml::CartesianPoint shipxml::Limit::P1() {
    return p1;
}

void shipxml::Limit::P1(shipxml::CartesianPoint p) {
    p1 = p;
}

void shipxml::Limit::LimitType(shipxml::LimitType l){
    limitType = l;
}

