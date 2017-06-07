#ifndef SPLIT_H
#define SPLIT_H

// splits an NMEA message into pieces, with the ability to query on them
// For a complete implementation see
// https://github.com/GobySoft/goby/blob/2.1/src/util/linebasedcomms/nmea_sentence.h
// https://github.com/GobySoft/goby/blob/2.1/src/util/linebasedcomms/nmea_sentence.cc

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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
