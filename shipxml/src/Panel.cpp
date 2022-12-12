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

std::string Panel::BlockName() { return blockName ; }
void Panel::BlockName(std::string b) {blockName=b;}
void Panel::Category(std::string s) {category=s;}
std::string Panel::Category() { return category; }
void Panel::CategoryDescription(std::string s) { categoryDescription=s;}
std::string Panel::CategoryDescription() { return categoryDescription; }
void Panel::Planar(bool b) {planar=b;}
bool Panel::Planar() { return planar; }
void Panel::Pillar(bool p) {pillar=p;}
bool Panel::Pillar() { return pillar; }
void Panel::Owner(std::string s) {owner=s;}
std::string Panel::Owner() { return owner; }
std::string Panel::DefaultMaterial() { return defaultMaterial; }
void Panel::Tightness(std::string s) {tightness=s;}
std::string Panel::Tightness() { return tightness; }
}  // namespace shipxml