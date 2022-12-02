//
// Created by cz on 01.12.22.
//


#include "../include/KeyValue.h"

namespace shipxml {

string KeyValue::Key() { return key; }
string KeyValue::Value() { return value; }
KeyValue::KeyValue(string const & k, string const & v) {

    key = k;
    value = v;
  }

}  // namespace shipxml