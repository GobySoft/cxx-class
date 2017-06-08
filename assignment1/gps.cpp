#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include "helpers.h"
    
int main(int argc, char* argv[])
{
   std::string line;
    
   //while(std::getline(std::cin, line))
   //{
     
   // std::cout << line << std::endl;


   std::string example = "$GPGGA,170834,4124.8963,N,08151.6838,W,1";
   
   //or NMEASentence nmea(line) to read in from the terminal
   NMEASentence nmea(example);
    
    if (nmea[0]  == "$GPGGA") {

    std:: string gpscode =  nmea.as<std::string>(0);


    int time  = nmea.as<int>(1);
    int time_in_secs = nmea_time_to_seconds(time);
   
    double nmea_lat = nmea.as<double>(2);
    char nmea_lat_hemi = nmea.as<char>(3);
    double dec_lat = nmea_geo_to_decimal(nmea_lat, nmea_lat_hemi);

    double nmea_long = nmea.as<double>(4);
    char nmea_long_hemi = nmea.as<char>(5);
    double dec_long = nmea_geo_to_decimal(nmea_long,nmea_long_hemi);

    int gps_fix = nmea.as<int>(6);

 

    std::cout << "GPGGA { seconds_since_midnight: "  << time_in_secs  << " latitude: "  << dec_lat << " longitude: " << dec_long << " fix_quality: " << gps_fix  <<std::endl;
    }
      // }   
}
