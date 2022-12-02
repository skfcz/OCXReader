//
// Created by Zerbst on 01.12.2022.
//

#include "../include/EntityWithProperties.h"

shipxml::EntityWithProperties::EntityWithProperties(string n) : NamedEntity(n){
  Name(n);
}

shipxml::Properties shipxml::EntityWithProperties::Properties() {
    return properties;
}
string shipxml::EntityWithProperties::Name() { return name; }
void shipxml::EntityWithProperties::Name(string n) { name = n; }
