//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_ENTITYWITHPROPERTIES_H
#define SHIPXML_ENTITYWITHPROPERTIES_H


#include "Properties.h"
#include "NamedEntity.h"
#include <string>
using namespace std;

namespace shipxml {
    class EntityWithProperties : public NamedEntity {

    public:
        EntityWithProperties(string n);

        shipxml::Properties Properties();

        string Name();

        void Name(string n);

    private:
        shipxml::Properties properties;
        string name;

    };

}
#endif //SHIPXML_ENTITYWITHPROPERTIES_H
