#include "gpssentence.h"

std::ostream& operator<<(std::ostream& os, const GPSPosition& gps_pos)
{
    return (os << gps_pos.sentence_id() << " { time: " << gps_pos.time() << " latitude: " << gps_pos.latitude() << " longitude: " << gps_pos.longitude() << " }");
}

