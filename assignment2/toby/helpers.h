#ifndef ASSIGNMENT1_HELPERS_H
#define ASSIGNMENT1_HELPERS_H

#include <vector>
#include <string>
#include <cmath>

// given a time in "NMEA form", returns the value as seconds since the start of the day
// NMEA form is HHMMSS.SSSS where "H" is hours, "M" is minutes, "S" is seconds or fractional seconds
inline double nmea_time_to_seconds(double nmea_time)
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
inline double nmea_geo_to_decimal(double nmea_geo, char hemi)
{
    // DDMM.MMMM
    double deg_int = std::floor(nmea_geo / 1e2);
    double deg_frac = (nmea_geo - (deg_int * 1e2)) / 60;

    double sign = 1;
    if(hemi == 'S' || hemi == 'W')
        sign = -1;
    
    return sign*(deg_int + deg_frac);
}



#endif
