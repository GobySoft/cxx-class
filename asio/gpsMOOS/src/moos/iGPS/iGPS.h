#ifndef IGPS_H
#define IGPS_H

#include "helpers.h"
#include "iGPS_config.pb.h"
#include "messages/gps.pb.h"
#include "goby/util/linebasedcomms/serial_client.h"
#include "goby/moos/goby_moos_app.h"

class iGPS : public GobyMOOSApp
{
 public:
  static iGPS* get_instance();
  static void delete_instance();
 private:
  iGPS(iGPSpb::iGPSConfig& cfg); // problem with using a reference?
  ~iGPS();
  void loop();
  static iGPS* inst_;
  iGPSpb::iGPSConfig& cfg_;
  goby::util::SerialClient serial_;
};

class GPSPosition : public NMEASentence
{
 public:
 GPSPosition(std::string arg) : NMEASentence(arg) {}
  int time();
  virtual double latitude() = 0;
  virtual double longitude() = 0;
  std::unique_ptr<iGPSpb::GPSMessage> makeMessage();
}; // C++ for some reason thinks GPSMessage does not belong to iGPSpb, but that
// is what I named the package in the protobuf file: wrong?
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
