
#ifndef GPSSENTENCE_H
#define GPSSENTENCE_H

#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <cmath>

#include "udp.pb.h"


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
    virtual ~NMEASentence() { }

    // e.g. "GGA"
    std::string sentence_id() const
    { return at(0).substr(3, 3); }
    
    // return a given field 'i' as a given type 'T'
    template<typename T>
        T as(int i) const noexcept
    {
        try { return boost::lexical_cast<T>(at(i)); }
        catch(...) { return T(); }
    }
};

class GPSPosition : public NMEASentence
{
public:
    GPSPosition(const std::string& s) : NMEASentence(s) { }
    virtual ~GPSPosition() { }    
    
    virtual int time() const = 0;
    virtual double latitude() const = 0;
    virtual double longitude() const = 0;

    void set_proto(protobuf::GPSPosition* proto_pos)
    {
        proto_pos->set_time(time());
        proto_pos->set_longitude(longitude());
        proto_pos->set_latitude(latitude());
    }    
    
};

std::ostream& operator<<(std::ostream& os, const GPSPosition& gps_pos);



class GGASentence : public GPSPosition
{
public:
    GGASentence(const std::string& s) : GPSPosition(s) { }
    virtual ~GGASentence() { }    
    
    int time() const override 
    { return nmea_time_to_seconds(this->as<double>(UTC)); }
    
    double latitude() const override
    { return nmea_geo_to_decimal(this->as<double>(LAT), this->as<char>(LAT_HEMI)); }
    
    double longitude() const override
    { return nmea_geo_to_decimal(this->as<double>(LON), this->as<char>(LON_HEMI)); }    

    enum class FixQuality { INVALID = 0, GPS = 1, DGPS = 2 };
    FixQuality fix_quality() const
    {
        int fix_quality = this->as<int>(FIX_QUALITY);
        if(fix_quality == 1)
            return FixQuality::GPS;
        else if(fix_quality == 2)
            return FixQuality::DGPS;
        else
            return FixQuality::INVALID;
    }
    
    
private:
    enum { TALKER = 0,
           UTC = 1,
           LAT = 2,
           LAT_HEMI = 3,
           LON = 4,
           LON_HEMI = 5,
           FIX_QUALITY = 6 };

};

class RMCSentence : public GPSPosition
{
public:
    RMCSentence(const std::string& s) : GPSPosition(s) {}
    virtual ~RMCSentence() { } 
    
    int time() const override 
    { return nmea_time_to_seconds(this->as<double>(UTC)); }

    double latitude() const override
    { return nmea_geo_to_decimal(this->as<double>(LAT), this->as<char>(LAT_HEMI)); }
    
    double longitude() const override
    { return nmea_geo_to_decimal(this->as<double>(LON), this->as<char>(LON_HEMI)); }    

    float sog_knots() const
    { return this->as<float>(SOG_KNOTS); }
    
    
private:
    enum { TALKER = 0,
           UTC = 1,
           RECEIVER_WARNING = 2,
           LAT = 3,
           LAT_HEMI = 4,
           LON = 5,
           LON_HEMI = 6,
           SOG_KNOTS = 7 };
        
};    

#endif
