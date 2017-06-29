#include "gpssentence.h"
#include <iomanip>
#include <limits>

std::ostream& operator<<(std::ostream& os, const GPSPosition& gps_pos)
{
    return (os << gps_pos.sentence_id() << " { time: " << gps_pos.time() << " latitude: " << std::setprecision(std::numeric_limits<double>::digits10) << gps_pos.latitude() << " longitude: " << gps_pos.longitude() << " }");
}

