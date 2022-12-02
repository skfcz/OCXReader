//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_ARCSEGMENT_H
#define SHIPXML_ARCSEGMENT_H

#include "CartesianPoint.h"
namespace shipxml {

    class ArcSegment {
     public:
      ArcSegment(CartesianPoint p0, CartesianPoint p1);
      ArcSegment(CartesianPoint p0, CartesianPoint p1, CartesianPoint pM, CartesianPoint pC, bool witherShins);

     private:
      CartesianPoint p0;
      CartesianPoint p1;
      CartesianPoint pM;
      CartesianPoint pC;
      bool witherShins;
      bool isLine;
    };

} // shipxml

#endif //SHIPXML_ARCSEGMENT_H
