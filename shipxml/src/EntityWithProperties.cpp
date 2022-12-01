//
// Created by Zerbst on 01.12.2022.
//

#include "EntityWithProperties.h"

shipxml::EntityWithProperties::EntityWithProperties(string n) : NamedEntity(n){

}

shipxml::Properties shipxml::EntityWithProperties::Properties() {
    return properties;
}
