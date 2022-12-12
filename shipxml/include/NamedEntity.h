//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_NAMEDENTITY_H
#define SHIPXML_NAMEDENTITY_H

#include <string>
using namespace std;

namespace  shipxml {
    class NamedEntity {

    public:
        NamedEntity(string n);

        string Name();
        void Name(string n);

    private:
        string name;

    };
}


#endif //SHIPXML_NAMEDENTITY_H
