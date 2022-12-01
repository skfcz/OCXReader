//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_EXTRUSION_H
#define SHIPXML_EXTRUSION_H

#include "EntityWithProperties.h"


namespace shipxml {
    class   Extrusion : EntityWithProperties {

    public:
        Extrusion();
        double Thickness();
        void Thickness(double t);
        double Offset();
        void Offset( double o);
        shipxml::Orientation Orientation();
        void Orientation( shipxml::Orientation o);

    private:
        double thickness;
        double offset;
        shipxml::Orientation orientation = UNDEFINED_O;

    };
}

#endif //SHIPXML_EXTRUSION_H
