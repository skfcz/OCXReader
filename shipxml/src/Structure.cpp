//
// Created by Zerbst on 01.12.2022.
//

#include "../include/Structure.h"

#include <iostream>
namespace shipxml {

void Structure::Add(shipxml::Panel p) { panels.push_back(p); }
}  // namespace shipxml