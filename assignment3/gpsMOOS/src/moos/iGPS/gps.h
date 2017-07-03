#ifndef GPS_H
#define GPS_H

#include "helpers.h"
#include "gps.pb.h"
#include "goby/util/linebasedcomms/serial_client.h"
#include "goby/moos/goby_moos_app.h"

class GPS : public GobyMOOSApp
{
 public:
  static GPS* get_instance();
  static void delete_instance();
 private:
  GPS(GPSConfig& cfg);
  ~GPS;
  void loop();
  static GPS* inst_;
  GPSConfig& cfg_;
  goby::util::SerialClient serial_;
};

class GPSPosition : public NMEASentence
{
 public:
 GPSPosition(std::string arg) : NMEASentence(arg) {}
  int time();
  virtual double latitude() = 0;
  virtual double longitude() = 0;
  std::unique_ptr<gps::GPSMessage> makeMessage();
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
