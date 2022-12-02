//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_PANEL_H
#define SHIPXML_PANEL_H

#include "EntityWithProperties.h"
#include "Extrusion.h"
#include "Support.h"
#include "Limit.h"
#include "AMCurve.h"
#include "enums.h"
#include <list>
using namespace std;

namespace shipxml {


    class Panel : public  EntityWithProperties {

    public:
        /**
         * Create new Panel Objects
         * @param n the name
         */
        Panel(string n);

        /**
         * Get the Extrusion
         */
        shipxml::Extrusion Extrusion();

        /**
         * Get the Support
         */
        shipxml::Support Support();

        /**
         * Get the list of Limits
         */
        list<shipxml::Limit> Limits();
        void Limits(list<shipxml::Limit> list);


        /**
         * Get the boundary curve (may be null)
         */
        shipxml::AMCurve Geometry();
        /**
         * Set the boundary curve
         */
        void Geometry(shipxml::AMCurve);

    private:
        shipxml::Extrusion extrusion= shipxml::Extrusion();
        shipxml::Support support = shipxml::Support();
        list<shipxml::Limit> limits;
        shipxml::AMCurve geometry = shipxml::AMCurve( AMSystem::XY);

    };

} // shipxml

#endif //SHIPXML_PANEL_H
