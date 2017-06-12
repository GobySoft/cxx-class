#include "gpssentence.h"
#include <iostream>
#include <iomanip>
#include <memory>


// mkdir build
// cd build
// cmake ..
// make
// cat ../../../assignment1/input.gps | ./gps-parser2

int main()
{
    
    std::string line;
    std::vector<std::unique_ptr<GPSPosition>> gps_positions;
    
    while(std::getline(std::cin, line))
    {

        enum { TALKER_LENGTH = 6 };
        if(line.size() > TALKER_LENGTH)
        {
            auto talker = line.substr(3,3);
            if(talker == "GGA")
                gps_positions.push_back(std::unique_ptr<GPSPosition>(new GGASentence(line)));
            else if(talker == "RMC")
                gps_positions.push_back(std::unique_ptr<GPSPosition>(new RMCSentence(line)));
        }
    }

    for(const auto& gps_pos : gps_positions)
        std::cout << *gps_pos << std::endl;    
}

