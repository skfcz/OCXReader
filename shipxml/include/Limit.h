//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_LIMIT_H
#define SHIPXML_LIMIT_H

#include "enums.h"
#include "AMCurve.h"
#include "CartesianPoint.h"
#include "NamedEntity.h"
#include <string>
using namespace std;

namespace shipxml {


    class Limit : NamedEntity {

    public:
        Limit(string n);

        shipxml::AMCurve Contour();
        void Contour(shipxml::AMCurve c);

        string Feature();
        void Feature(string s);

        shipxml::LimitType LimitType();
        void LimitType( shipxml::LimitType l);

        double Offset();
        void Offset( double o);

        shipxml::CartesianPoint P0();
        void  P0(shipxml::CartesianPoint p);

        shipxml::CartesianPoint P1();
        void  P1(shipxml::CartesianPoint p);


    private:
        shipxml::AMCurve contour =shipxml::AMCurve(XY);
        string feature;
        shipxml::LimitType limitType = UNKNOWN;
        double offset;
        shipxml::CartesianPoint p0;
        shipxml::CartesianPoint p1;

    };
}

#endif //SHIPXML_LIMIT_H
