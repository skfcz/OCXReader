//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_AMCURVE_H
#define SHIPXML_AMCURVE_H

#include <list>
#include <string>
#include "ArcSegment.h"
#include "enums.h"
using namespace std;

namespace shipxml {


    class AMCurve {
    public:
        AMCurve(shipxml::AMSystem s);

        list<shipxml::ArcSegment> Segments();

        shipxml::AMSystem System();
    private:
        list<shipxml::ArcSegment> segments;
        AMSystem system;
    };
}

#endif //SHIPXML_AMCURVE_H
