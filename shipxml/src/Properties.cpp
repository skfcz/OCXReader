//
// Created by Zerbst on 01.12.2022.
//

#include "../include/Properties.h"
shipxml::KeyValue shipxml::Properties::Add(const std::string key, const std::string value) {
  // TODO: do not add twice

  KeyValue kv( key, value);
  values.push_back(kv);
  return kv;
}
shipxml::KeyValue shipxml::Properties::Add(std::string key, double value) {

  return Add(key, std::to_string(value));

}
shipxml::KeyValue shipxml::Properties::Add(std::string key, int value) {
  return Add(key, std::to_string(value));
}
