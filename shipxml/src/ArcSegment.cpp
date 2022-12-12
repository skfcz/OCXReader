//
// Created by Zerbst on 01.12.2022.
//

#include "../include/ArcSegment.h"

namespace shipxml {
ArcSegment::ArcSegment(CartesianPoint pS, CartesianPoint pE) {
  p0 = pS;
  p1 = pE;
  isLine=true;
}
ArcSegment::ArcSegment(CartesianPoint pS, CartesianPoint pE, CartesianPoint pOnCircle,
                       CartesianPoint pCenter, bool counterClockWise) {

  p0 = pS;
  p1=pE;
  pM=pOnCircle;
  pC =pCenter;
  isLine=false;
  witherShins = counterClockWise;
  // calculate radius and amplitude
}
} // shipxml