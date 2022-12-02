//
// Created by Zerbst on 01.12.2022.
//

#include "../include/Panel.h"
namespace shipxml {

Panel::Panel(string n) : EntityWithProperties(n) { Name(n); }

Extrusion Panel::Extrusion() { return extrusion; }

shipxml::Support Panel::Support() { return support; }

list<shipxml::Limit> Panel::Limits() { return limits; }

shipxml::AMCurve Panel::Geometry() { return geometry; }

void Panel::Geometry(shipxml::AMCurve c) { geometry = c; }
void Panel::Limits(list<shipxml::Limit> l) { limits = l; }
}  // namespace shipxml