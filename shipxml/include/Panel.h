//
// Created by Zerbst on 01.12.2022.
//

#ifndef SHIPXML_PANEL_H
#define SHIPXML_PANEL_H

#include <list>

#include "AMCurve.h"
#include "EntityWithProperties.h"
#include "Extrusion.h"
#include "Limit.h"
#include "Support.h"
#include "enums.h"
using namespace std;

namespace shipxml {

class Panel : public EntityWithProperties {
 public:
  /**
   * Create new Panel Objects
   * @param n the name
   */
  Panel(string n);

  void BlockName(std::string s);
  std::string BlockName();

  void Category(std::string s);
  std::string Category();

  void CategoryDescription(std::string s);
  std::string CategoryDescription();

  void Planar(bool p);
  bool Planar();

  void Pillar(bool p);
  bool Pillar();

  void Owner(std::string s);
  std::string Owner();

  void DefaultMaterial(std::string s);
  std::string DefaultMaterial();

  void Tightness(std::string s);
  std::string Tightness();

  /**
   * Get the Extrusion
   */
  shipxml::Extrusion Extrusion();

  /**
   * Get the Support
   */
  shipxml::Support Support();

  /**
   * Get the list of Limits
   */
  list<shipxml::Limit> Limits();
  void Limits(list<shipxml::Limit> list);

  /**
   * Get the boundary curve (may be null)
   */
  shipxml::AMCurve Geometry();
  /**
   * Set the boundary curve
   */
  void Geometry(shipxml::AMCurve);

 private:
  std::string blockName="";
  std::string category="";
  std::string categoryDescription="";
  bool planar;
  bool pillar;
  std::string owner = "";
  std::string defaultMaterial="";
  std::string tightness="";
  shipxml::Extrusion extrusion = shipxml::Extrusion();
  shipxml::Support support = shipxml::Support();
  list<shipxml::Limit> limits;
  shipxml::AMCurve geometry = shipxml::AMCurve(AMSystem::XY);
};

}  // namespace shipxml

#endif  // SHIPXML_PANEL_H
