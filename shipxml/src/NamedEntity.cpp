//
// Created by Zerbst on 01.12.2022.
//

#include "NamedEntity.h"



string shipxml::NamedEntity::Name() {
    return name;
}

void shipxml::NamedEntity::Name(string n) {
    name = n;
}

shipxml::NamedEntity::NamedEntity(string n) {
    name=n;

}
