#include "helpers.h"
#include "dccl.h"
#include "iGPS.h"
#include "iGPS_config.pb.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace goby::common::logger;

boost::shared_ptr<iGPSpb::GPSConfig> master_config;
iGPS* iGPS::inst_ = 0;

int GPSPosition :: time()
{
  return nmea_time_to_seconds(this->as<int>(1));
}

std::unique_ptr<iGPSpb::GPSMessage> GPSPosition :: makeMessage()
// "'GPSMessage is not a member of 'iGPSpb'" for some reason.
{
  iGPSpb::GPSMessage* result = new iGPSpb::GPSMessage();
  // "'result' was not declared in this scope": I thought this line WAS the
  // declaration? If not, try declaring it on another line?
  // Could be a result of problems with iGPS namespace.
  // There was also a bug in this line that I fixed; could be the cause.
  // Also, "expected type-specifier" on the right side of the assignment. What?
  result->set_time(this->time());
  result->set_latitude(this->latitude());
  result->set_longitude(this->longitude());
  return std::unique_ptr<iGPSpb::GPSMessage>(result);
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

iGPS* iGPS::get_instance()
{
  if(!inst_)
    {
      master_config->reset(new iGPSpb::iGPSConfig);
      inst_ = new iGPS(*master_config);
    }
  return inst_;
}

void iGPS::delete_instance()
  delete inst_;
  // "error: expected initializer before 'delete'"
  // But the point of this function is to delete something that already exists?
  // Maybe some conditional needs to check that inst_ is initialized before
  // trying to delete it.
} // "expected declaration before '}' token": what would I declare in a function
  // that returns void?

iGPS::iGPS(iGPSConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    serial_(cfg_.serial_port(), cfg_.serial_baud())
{
  serial_.start();
}

iGPS::~iGPS()
{
  serial_.close();
}

void iGPS::loop()
{

  std::string in;
  while(serial_.readline(&line))
    {
      if(line.empty()) continue;

      // relocated; purpose not quite clear, I'm assuming they're necessary
      // before I can start making and parsing DCCL GPSMessages
      dccl::Codec dccl;
      dccl.load<iGPSpb::GPSMessage>();

      // declare variable
      std::unique_ptr<iGPSpb::GPSMessage> msg;
      
      if (in.substr(3,3)=="GGA") {
	GGASentence* ptrgga = new GGASentence(in); // necessity of pointer?
	msg = ptrgga->makeMessage(); // problem: msg needs pointer?
	delete ptrgga;
      } else if (in.substr(3,3)=="RMC") {
	RMCSentence ptrrmc = new RMCSentence(in);
	msg = ptrrmc.makeMessage();
	delete ptrrmc;
      }
      // debugging
      std::cout << msg.ShortDebugString() << std::endl;
      publish_pb("GPS_MESSAGE",*msg);
    }

}

int main(int argc, char* argv[]) {

  return goby::moos::run<GPS>(argc,argv); // should this be iGPS?

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

