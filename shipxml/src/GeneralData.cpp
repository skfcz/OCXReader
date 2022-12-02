//
// Created by cz on 02.12.22.
//
#include "../include/GeneralData.h"
std::string shipxml::GeneralData::ShipName() { return shipName; }
void shipxml::GeneralData::ShipName(std::string s) { shipName = s; }
std::string shipxml::GeneralData::YardNumber() { return yardNumber; }
void shipxml::GeneralData::YardNumber(std::string s) { yardNumber = s; }
double shipxml::GeneralData::AftEnd() { return aftEnd; }
void shipxml::GeneralData::AftEnd(double d) { aftEnd = d; }
double shipxml::GeneralData::AftPerpendicular() { return aftPerpendicular; }
void shipxml::GeneralData::AftPerpendicular(double d) { aftPerpendicular = d; }
double shipxml::GeneralData::ForwardEnd() { return forwardEnd; }
void shipxml::GeneralData::ForwardEnd(double d) { forwardEnd = d; }
double shipxml::GeneralData::ForwardPerpendicular() {
  return forwardPerpendicular;
}
void shipxml::GeneralData::ForwardPerpendicular(double d) {
  forwardPerpendicular = d;
}
double shipxml::GeneralData::LengthOverall() { return lengthOverall; }
void shipxml::GeneralData::LengthOverall(double d) { lengthOverall = d; }
double shipxml::GeneralData::MaxDraught() { return maxDraught; }
void shipxml::GeneralData::MaxDraught(double d) { maxDraught = d; }
double shipxml::GeneralData::MaxBreadth() { return maxBreadth; }
void shipxml::GeneralData::MaxBreadth(double d) { maxBreadth = d; }
double shipxml::GeneralData::MaxHeight() { return maxHeight; }
void shipxml::GeneralData::MaxHeight(double d) { maxHeight = d; }
double shipxml::GeneralData::HeightOfMainDeck() { return heightOfMainDeck; }
void shipxml::GeneralData::HeightOfMainDeck(double d) { heightOfMainDeck = d; }
