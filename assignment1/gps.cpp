#include <iostream>
#include "helpers.h"

enum Quality { INV=0, GPS=1, DGPS=2 };

struct GPGGA {
  int time;
  double latitude;
  double longitude;
  Quality fix;
public:
  GPGGA(NMEASentence sen) {
    time = nmea_time_to_seconds(stoi(sen[1]));
    latitude = nmea_geo_to_decimal(stod(sen[2]),sen[3][0]);
    longitude = nmea_geo_to_decimal(stod(sen[4]),sen[5][0]);
    fix = static_cast<Quality>(stoi(sen[6]));
  }
};
  
int main() {
  std::string in;
  while(getline(std::cin,in)) {
    NMEASentence nmea(in);
    std::string prefix = nmea[0];
    if (prefix=="$GPGGA") {
	prefix = prefix.substr(1);
	GPGGA gpgga(nmea);
	std::string fixquality;
	if (gpgga.fix==Quality::INV) {
	  fixquality = "INVALID";
	} else if (gpgga.fix==Quality::GPS) {
	  fixquality = "GPS_FIX";
	} else if (gpgga.fix==Quality::DGPS) {
	  fixquality = "DGPS_FIX";
	}
	std::cout << prefix << " { seconds_since_midnight: " << gpgga.time << " latitude: " << gpgga.latitude << " longitude: " << gpgga.longitude << " fix_quality: " << fixquality << " }" << std::endl;
    }
  }
}
