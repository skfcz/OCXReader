//
// Created by cz on 01.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_KEYVALUE_H
#define OCXREADER_DISTRIBUTION_KEYVALUE_H

#include <string>
using std::string;
namespace shipxml {

class KeyValue {
 public:
  KeyValue(string const & key, string const & value);

  string Key();
  string Value();

 private:
  string key;
  string value;


};

}  // namespace shipxml

#endif  // OCXREADER_DISTRIBUTION_KEYVALUE_H
