#include "helpers.h"
#include "dccl.h"
#include "gps.h"
#include "gps.pb.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace goby::common::logger;

boost::shared_ptr<GPSConfig> master_config;
GPS* GPS::inst_ = 0;

int GPSPosition :: time()
{
  return nmea_time_to_seconds(this->as<int>(1));
}

std::unique_ptr<gps::GPSMessage> GPSPosition :: makeMessage()
{
  gps::GPSMessage* result = new gps::GPSMessage();
  result->set_time(this->time());
  result->set_latitude(this->latitude());
  result->set_longitude(this->longitude());
  return std::unique_ptr<gps::GPSMessage>(result);
  // I think nothing needs to be deleted here because the GPSMessage* created
  // with a "new" is eventually used to make a smart pointer. So I think the
  // smart pointer will handle the eventual deletion.
}

double GGASentence :: latitude()
{
  return nmea_geo_to_decimal(this->as<double>(2),this->as<char>(3));
}

double GGASentence :: longitude()
{
  return nmea_geo_to_decimal(this->as<double>(4),this->as<char>(5));
}

GGASentence::Quality GGASentence :: fix()
{
  return static_cast<Quality>(this->as<int>(6));
}

double RMCSentence :: latitude()
{
  return nmea_geo_to_decimal(this->as<double>(3),this->as<char>(4));
}

double RMCSentence :: longitude()
{
  return nmea_geo_to_decimal(this->as<double>(5),this->as<char>(6));
}

double RMCSentence :: speedOverGround()
{
  return this->as<double>(7);
}

GPS* GPS::get_instance()
{
  if(!inst_)
    {
      master_config.reset(new GPSConfig);
      inst_ = new GPS(*master_config);
    }
  return inst_;
}

void GPS::delete_instance()
  delete inst_;
}

GPS::GPS(GPSConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    serial_(cfg_.serial_port(), cfg_.serial_baud())
{
  serial_.start();
}

GPS::~GPS()
{
  serial_.close();
}

void GPS::loop()
{

  std::string in;
  while(serial_.readline(&line))
    {
      if(line.empty()) continue;

      // relocated; purpose not quite clear, I'm assuming they're necessary
      // before I can start making and parcing DCCL GPSMessages
      dccl::Codec dccl;
      dccl.load<gps::GPSMessage>();

      // declare variable
      std::unique_ptr<gps::GPSMessage> msg;
      
      if (in.substr(3,3)=="GGA") {
	GGASentence* ptrgga = new GGASentence(in); // necessity of pointer?
	msg = ptrgga->makeMessage();
	delete ptrgga;
      } else if (in.substr(3,3)=="RMC") {
	RMCSentence ptrrmc = new RMCSentence(in);
	msg = ptrrmc.makeMessage();
	delete ptrrmc;
      }
      // debugging
      std::cout << msg.ShortDebugString() << std::endl;
      publish_pb("GPS_MESSAGE",msg);
    }

}

int main(int argc, char* argv[]) {

  return goby::moos::run<CTDSimple>(argc,argv);

/* The main function from the old GPS program that used socat.
  std::string in;
  std::vector<std::unique_ptr<gps::GPSMessage>> GPSvec;
  while(getline(std::cin,in))
    {
      if (in.substr(3,3)=="GGA") {
	GGASentence* ptrgga = new GGASentence(in);
	GPSvec.push_back(ptrgga->makeMessage());
	delete ptrgga;
      } else if (in.substr(3,3)=="RMC") {
	RMCSentence* ptrrmc = new RMCSentence(in);
	GPSvec.push_back(ptrrmc->makeMessage());
	delete ptrrmc;
      }
    }
  dccl::Codec dccl;
  dccl.load<gps::GPSMessage>();
  double total_byte_size = 0.0;
  for( std::unique_ptr<gps::GPSMessage> const& msg : GPSvec )
    {
      std::cout << msg->DebugString() <<  std::endl;
      total_byte_size += dccl.size(*msg);
    }
  std::cout << "Avg. # of bytes necessary: " << (total_byte_size/((double)GPSvec.size())) << std::endl;
  return 0;
*/
}

