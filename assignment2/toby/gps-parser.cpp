#include "gpssentence.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include "dccl.h"

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

    
    dccl::Codec dccl;
    dccl.load<protobuf::GPSPosition>();
    
    double proto_size_sum = 0;
    double dccl_size_sum = 0;
    for(const auto& gps_pos : gps_positions)
    {
        std::cout << *gps_pos << std::endl;
        protobuf::GPSPosition proto_pos;
        gps_pos->set_proto(&proto_pos);
        std::cout << "Protobuf (" << proto_pos.ByteSize() << "B): " << proto_pos.ShortDebugString() << std::endl;
        proto_size_sum += proto_pos.ByteSize();

        // encode / decode via DCCL to mimic transmission to another destination
        std::string bytes;
        dccl.encode(&bytes, proto_pos);
        protobuf::GPSPosition dccl_pos_out;
        dccl.decode(bytes, &dccl_pos_out);
        std::cout << "DCCL (" << dccl.size(proto_pos) << "B): " << dccl_pos_out.ShortDebugString() << "\n" << std::endl;
        
        dccl_size_sum += dccl.size(proto_pos);
    }

    double proto_mean = proto_size_sum / gps_positions.size();
    std::cout << "Protobuf mean size: " << proto_mean << "B" << std::endl;
    double dccl_mean = dccl_size_sum / gps_positions.size();
    std::cout << "DCCL mean size: " << dccl_mean << "B" << std::endl;
}

