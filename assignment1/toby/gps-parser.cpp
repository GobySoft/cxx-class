#include "../helpers.h"
#include <iostream>
#include <iomanip>

// g++ gps-parser.cpp -o gps-parser --std=c++11
// cat ../input.gps | ./gps-parser


int main()
{
    struct GPGGAPartial
    {
        int time_since_day_start;
        double lat;
        double lon;
        enum class FixQuality { INVALID = 0, GPS = 1, DGPS = 2 };
        FixQuality fix_quality;
        
    };

    std::string line;
    while(std::getline(std::cin, line))
    {
        enum { TALKER = 0,
               UTC = 1,
               LAT = 2,
               LAT_HEMI = 3,
               LON = 4,
               LON_HEMI = 5,
               FIX_QUALITY = 6
        };
        
        NMEASentence nmea(line);

        // not a $GPGGA
        if(nmea.as<std::string>(TALKER) != "$GPGGA")
            continue;

        GPGGAPartial gpgga;
        gpgga.time_since_day_start = nmea_time_to_seconds(nmea.as<double>(UTC));
        gpgga.lat = nmea_geo_to_decimal(nmea.as<double>(LAT), nmea.as<char>(LAT_HEMI));
        gpgga.lon = nmea_geo_to_decimal(nmea.as<double>(LON), nmea.as<char>(LON_HEMI));

        int fix_quality = nmea.as<int>(FIX_QUALITY);
        if(fix_quality == 1)
            gpgga.fix_quality = GPGGAPartial::FixQuality::GPS;
        else if(fix_quality == 2)
            gpgga.fix_quality = GPGGAPartial::FixQuality::DGPS;
        else
            gpgga.fix_quality = GPGGAPartial::FixQuality::INVALID;

        std::cout << line << std::endl;
        std::cout << "GPGGA { seconds_since_midnight: " << gpgga.time_since_day_start << " latitude: " << std::setprecision(std::numeric_limits<double>::digits10) <<  gpgga.lat << " longitude: " << gpgga.lon <<  " fix_quality: ";

        switch(gpgga.fix_quality)
        {
            case GPGGAPartial::FixQuality::GPS: std::cout << "GPS"; break;
            case GPGGAPartial::FixQuality::DGPS: std::cout << "DGPS"; break;
            case GPGGAPartial::FixQuality::INVALID: std::cout << "INVALID"; break;
        }

        std::cout << " }" << std::endl;
        
    }
    
}

