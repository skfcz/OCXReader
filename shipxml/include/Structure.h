//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_STRUCTURE_H
#define SHIPXML_STRUCTURE_H

#include "Panel.h"
#include <list>
using namespace std;

namespace shipxml {

    class Structure {

    public:
        list<Panel> Panels();
        void Panels( list<shipxml::Panel> l);

    private:
        list<shipxml::Panel> panels;
    };

} // shipxml

#endif //SHIPXML_STRUCTURE_H
