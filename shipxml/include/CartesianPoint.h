//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_CARTESIANPOINT_H
#define SHIPXML_CARTESIANPOINT_H

namespace shipxml {

    class CartesianPoint {

    public:
        CartesianPoint();
        CartesianPoint(double x, double y, double z);

        double X();
        void X(double d);

        double Y();
        void Y(double d);

        double Z();
        void Z(double d);

    private:
        double x;
        double y;
        double z;

    };

} // shipxml

#endif //SHIPXML_CARTESIANPOINT_H
