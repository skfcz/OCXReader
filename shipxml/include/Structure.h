//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_STRUCTURE_H
#define SHIPXML_STRUCTURE_H

#include "Panel.h"
#include "Bracket.h"
#include <list>
#include <list>
using namespace std;

namespace shipxml {

    class Structure {

    public:
        list<Panel> Panels();
        list<Bracket> Brackets();

    private:
        list<Panel> panels;
        list<Bracket> brackets;

    };

} // shipxml

#endif //SHIPXML_STRUCTURE_H
