//
// Created by cz on 30.11.22.
//

#ifndef OCXREADER_DISTRIBUTION_SHIPXML_H
#define OCXREADER_DISTRIBUTION_SHIPXML_H

#endif  // OCXREADER_DISTRIBUTION_SHIPXML_H

#include <XmlObjMgt.hxx>
#include <gp_Pnt.hxx>
#include <list>

using std::list;
using std::string;

namespace shipxml {

class ShipSteelTransfer {
 public:
  Panel AddPanel(Panel panel;
  list<Panel> Panels();

private:
                 list<Panel> panels;
};

public KeyValue {
 public:
  KeyValue(string key, string value);
  string Key();
  string Value();

 private:
  string key;
  string value;
};

class Named_Entity {
 public:
  void Name(string name);

 private:
  string name;
};

class EntityWithProperties : Named_Entity {
 public:
  void BoundingBox(gp_Pnt lower, gp_Pnt upper);
  void COG(gp_Pnt cog);
  KeyValue AddProperty(string key, string value);
  KeyValue AddProperty(string key, double value);
  KeyValue AddProperty(string key, int value);

 private:
  gp_Pnt lower(1, 1, 1);
  gp_Pnt upper(-1, -1, -1);
  gp_Pnt cog(0, 0, 0);
  list<KeyValue> properties;
};

public
class Panel : EntityWithProperies {
 public:
  AMCurve Curve(AMCurve curve);
  AMCurve Curve();

  Limit AddLimit(Limit lim);
  list<Limit> Limits();

 private:
  AMCurve curve;
  list<Limit> limits;
} enum AMSystem { XY, XZ, YZ };

public
class AMCurve {
  AMSystem System(AMSystem);
  AMSystem System();
  ArcSegment AddArc(ArcSegment arc);
  list<ArcSegment> Segments();

 private:
  AMSystem system;
  list<ArcSegment> segments;
}

public ArcSegment {
 public:
  ArcSegment(gp_Pnt p0, gp_Pnt p1);
  ArcSegment(gp_Pnt p0, gp_Pnt p1, gp_Pnt pM, gp_Pnt pC, bool witherShins);

 private:
  gp_Pnt p0;
  gp_Pnt p1;
  gp_Pnt pM;
  gp_Pnt pC;
  bool witherShins;
}

};  // namespace shipxml