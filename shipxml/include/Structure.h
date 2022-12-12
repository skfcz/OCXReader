//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_STRUCTURE_H
#define SHIPXML_STRUCTURE_H

#include "Panel.h"
#include <list>
#include <iterator>
using namespace std;

namespace shipxml {

    class Structure {

    public:

     void Add( shipxml::Panel   p) ;
     std::list< shipxml::Panel> Panels();

    private:
       list<shipxml::Panel  > panels;
    };

} // shipxml

#endif //SHIPXML_STRUCTURE_H
