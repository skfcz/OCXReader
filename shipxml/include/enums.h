//
// Created by cz on 01.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_ENUMS_H
#define OCXREADER_DISTRIBUTION_ENUMS_H



namespace  shipxml {
  enum AMSystem {XY, XZ, YZ};
  enum LimitType {UNKNOWN, CURVE, TRACE, FOR_FLANGE,BLOCK, PANEL,PANEL_SAME_PLANE,  STIFFENER, KNUCKLE, GAP,HOLE  };
  enum PanelType { PLANAR,KNUCKLED, SHELL_PANEL };
  enum Orientation  {UNDEFINED_O, FORE, AFT,PS,SB,TOP,BOTTOM};
  enum LocationType {UNKNOWN_L, X_L,Y_L,Z_L, TP_L };
  enum MajorPlane {UNDEFINED_M, X_M,Y_M,Z_M};

}
#endif  // OCXREADER_DISTRIBUTION_ENUMS_H