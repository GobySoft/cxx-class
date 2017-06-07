#include <iostream> // for std::cout
#include <iomanip> // for std::setprecision
#include <limits> // for std::numeric_limits
#include "helpers.h" // for NMEASentence and nmea_geo_to_decimal


// compile with
// g++ helpers-usage.cpp --std=c++11 -o helpers-usage

// run with
// ./helpers-usage
    
// output should be 
//   Talker: $TALKR
//   int in position 1: 4
//   double in position 1: 3.2

int main()
{
    std::string example = "$TALKR,4,3.2,45*14";

    NMEASentence nmea(example);

    enum { TALKER = 0, SOMEINT = 1, SOMEFLOAT = 2, UNUSED = 3 };
    
    std::cout << "Talker: " <<  nmea.as<std::string>(TALKER) << std::endl;

    int someint = nmea.as<int>(SOMEINT);
    std::cout << "int in position 1: " << someint << std::endl;

    double somefloat = nmea.as<double>(SOMEFLOAT);
    std::cout << "double in position 1: " << somefloat << std::endl;


    // 70 degrees, 40.15 minutes West
    double nmea_lat = 7040.15;
    char nmea_lat_hemi = 'W';
    double dec_lat = nmea_geo_to_decimal(nmea_lat, nmea_lat_hemi);
    std::cout << std::setprecision(std::numeric_limits<double>::digits10) << "Decimal degrees: " << dec_lat << std::endl;
    
}

