//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_PROPERTIES_H
#define SHIPXML_PROPERTIES_H

#include "KeyValue.h"
#include <list>
using namespace std;

namespace  shipxml {
    class Properties {

     public:
        KeyValue Add( std::string key, std::string value);
        KeyValue Add( std::string key, double value);
        KeyValue Add( std::string key, int value);
        list<KeyValue> Values();

       private:
        list<KeyValue> values;
    };
}


#endif //SHIPXML_PROPERTIES_H
