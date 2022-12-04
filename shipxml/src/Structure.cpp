//
// Created by Zerbst on 01.12.2022.
//

#include "../include/Structure.h"
#include <iostream>
namespace shipxml {
    list<Panel> Structure::Panels() {
        return panels;
    }
void Structure::Panels(list<Panel> l) {

  cout << "Set Panels #" << l.size() << "\n";
  panels=l;
  cout << "After Set Panels #" << panels.size() << "\n";

}


} // shipxml