#ifndef GPS_H
#define GPS_H

#include "helpers.h"

class GPSPosition : public NMEASentence
{
 public:
 GPSPosition(std::string arg) : NMEASentence(arg) {}
  int time();
  virtual double latitude() = 0;
  virtual double longitude() = 0;
};

class GGASentence : public GPSPosition
{
 public:
 GGASentence(std::string arg) : GPSPosition(arg) {}
  enum Quality { INV=0, GPS=1, DGPS=2 };
  double latitude() override;
  double longitude() override;
  Quality fix();
  
};

class RMCSentence : public GPSPosition
{
 public:
 RMCSentence(std::string arg) : GPSPosition(arg) {}
  double latitude() override;
  double longitude() override;
  double speedOverGround();
};

#endif
