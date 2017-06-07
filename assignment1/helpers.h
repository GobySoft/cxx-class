#ifndef ASSIGNMENT1_HELPERS_H
#define ASSIGNMENT1_HELPERS_H

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


// given a time in "NMEA form", returns the value as seconds since the start of the day
// NMEA form is HHMMSS.SSSS where "H" is hours, "M" is minutes, "S" is seconds or fractional seconds
double nmea_time_to_seconds(double nmea_time)
{
    double hours = std::floor(nmea_time / 1e4);
    nmea_time -= hours*1e4;
    double minutes = std::floor(nmea_time / 1e2);
    nmea_time -= minutes*1e2;
    double seconds = nmea_time;
    
    return hours*3600 + minutes*60 + seconds;    
}

// given a latitude or longitude in "NMEA form" and the hemisphere character ('N', 'S', 'E' or 'W'), returns the value as decimal degrees
// NMEA form is DDDMM.MMMM or DDMM.MMMM where "D" is degrees, and "M" is minutes
double nmea_geo_to_decimal(double nmea_geo, char hemi)
{
    // DDMM.MMMM
    double deg_int = std::floor(nmea_geo / 1e2);
    double deg_frac = (nmea_geo - (deg_int * 1e2)) / 60;

    double sign = 1;
    if(hemi == 'S' || hemi == 'W')
        sign = -1;
    
    return sign*(deg_int + deg_frac);
}

// splits an NMEA message into pieces, with the ability to query on them
// For a complete implementation see
// https://github.com/GobySoft/goby/blob/2.1/src/util/linebasedcomms/nmea_sentence.h
// https://github.com/GobySoft/goby/blob/2.1/src/util/linebasedcomms/nmea_sentence.cc
class NMEASentence : public std::vector<std::string>
{
public:
    NMEASentence(std::string s)
    {
        // Check if the checksum exists and is correctly placed, and strip it.
        if (s.size() > 3 && s.at(s.size()-3) == '*') {
            std::string hex_csum = s.substr(s.size()-2);
            s = s.substr(0, s.size()-3);
        }

        // split string into vector on the commas
        boost::split(*(std::vector<std::string>*)this, s, boost::is_any_of(","));
    }
    

    // return a given field 'i' as a given type 'T'
    template<typename T>
        T as(int i) const noexcept
    {
        try { return boost::lexical_cast<T>(at(i)); }
        catch(...) { return T(); }
    }
};


#endif
