#include "helpers.h"
#include "gps.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

int GPSPosition :: time()
{
  return nmea_time_to_seconds(this->as<int>(1));
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

int main() {
  std::string in;
  std::vector<std::unique_ptr<GPSPosition>> GPSvec;
  while(getline(std::cin,in))
    {
      if (in.substr(3,3)=="GGA") {
	GGASentence* gpgga = new GGASentence(in);
	GPSvec.push_back(std::unique_ptr<GPSPosition>(gpgga));
      } else if (in.substr(3,3)=="RMC") {
	RMCSentence* gprmc = new RMCSentence(in);
	GPSvec.push_back(std::unique_ptr<GPSPosition>(gprmc));
      }
    }
  if (GPSvec[0]==GPSvec[1]) std::cout << "!" << std::endl;
  for( std::unique_ptr<GPSPosition> const& pos : GPSvec )
    {
      std::cout << pos->as<std::string>(0).substr(3) << " { time: " << pos->time() << std::setprecision(std::numeric_limits<double>::digits10) << " latitude: " << pos->latitude() << " longitude: " << pos->longitude() << " } " <<  std::endl;
    }
  return 0;
}

