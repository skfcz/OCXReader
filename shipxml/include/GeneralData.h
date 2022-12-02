//
// Created by cz on 02.12.22.
//

#ifndef OCXREADER_DISTRIBUTION_GENERALDATA_H
#define OCXREADER_DISTRIBUTION_GENERALDATA_H
#include <string>

namespace shipxml {
class GeneralData {
 public:
  std::string ShipName();
  void ShipName(std::string s);

  std::string YardNumber();
  void YardNumber(std::string s);

  double AftEnd();
  void AftEnd(double d);

  double AftPerpendicular();
  void AftPerpendicular(double d);

  double ForwardEnd();
  void ForwardEnd(double d);

  double ForwardPerpendicular();
  void ForwardPerpendicular(double d);

  double LengthOverall();
  void LengthOverall(double d);

  double MaxDraught();
  void MaxDraught(double d);

  double MaxBreadth();
  void MaxBreadth(double d);

  double MaxHeight();
  void MaxHeight(double d);

  double HeightOfMainDeck();
  void HeightOfMainDeck(double d);

 private:
  std::string shipName;
  std::string yardNumber;
  double aftEnd = 0.0;
  double aftPerpendicular = 0.0;
  double forwardEnd = 0.0;
  double forwardPerpendicular = 0.0;
  double lengthOverall = 0.0;
  double maxDraught = 0.0;
  double maxBreadth = 0.0;
  double maxHeight = 0.0;
  double heightOfMainDeck = 0.0;
};
}  // namespace shipxml
#endif  // OCXREADER_DISTRIBUTION_GENERALDATA_H
