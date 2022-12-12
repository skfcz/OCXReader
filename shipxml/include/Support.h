//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_SUPPORT_H
#define SHIPXML_SUPPORT_H

#include <string>
#include "CartesianPoint.h"
#include "enums.h"
using namespace std;

namespace shipxml {


    class Support {
    public:

        string Grid();
        void Grid(string g);

        string Coordinate();
        void Coordinate(string g);

        shipxml::Orientation Orientation();
        void Orientation( shipxml::Orientation o);

        bool Planar();
        void Planar(bool p);

        shipxml::LocationType LocationType();
        void LocationType(shipxml::LocationType l);

        shipxml::MajorPlane  MajorPlane();
        void MajorPlane(shipxml::MajorPlane l);

        CartesianPoint TP1();
        CartesianPoint TP2();
        CartesianPoint TP3();

        void TP1( shipxml::CartesianPoint p);
        void TP2( shipxml::CartesianPoint p);
        void TP3( shipxml::CartesianPoint p);


    private:
        string grid;
        string coordinate;
        shipxml::Orientation orientation = UNDEFINED_O;
        bool planar;
        shipxml::LocationType locationType=UNKNOWN_L;
        shipxml::MajorPlane majorPlane=UNDEFINED_M;
        shipxml::CartesianPoint tp1;
        shipxml::CartesianPoint tp2;
        shipxml::CartesianPoint tp3;
    };
}

#endif //SHIPXML_SUPPORT_H
