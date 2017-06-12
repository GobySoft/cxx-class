#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include "ctd.pb.h"
#include "dccl.h"

void show_vector()
{
    std::vector<int> ints { 4,5,6 };    
    for(int i = 0, n = ints.size(); i < n; ++i)
    {
        std::cout << ints[i] << std::endl;    
    }

    for(int v : ints)
    {
        std::cout << v << std::endl;
    }
}

void enums()
{
    enum Color 
    {
        COLOR_RED = 0,
        COLOR_BLUE = 1,
        COLOR_GREEN = 2
    };
    
    Color c = COLOR_RED;
    Color d = COLOR_BLUE;
    
    int i = c;
    
    enum class Color2
    {
        RED, BLUE, GREEN
    };

    Color2 e = Color2::RED;
    Color2 f = Color2::GREEN;
    int j = static_cast<int>(e);
    
}

// returns theindex of the next value
int parse_conductivity(const std::string& line, protobuf::CTDSample* ctd_ptr)
{
    std::string::size_type space_pos = line.find(' ');
    if(space_pos != std::string::npos)
    {        
        std::cout << space_pos << " is the index of the first space" << std::endl;
        std::string str_conductivity = line.substr(0, space_pos);
        std::cout << "C (as str): " << str_conductivity << std::endl;
        ctd_ptr->set_conductivity(std::stoi(str_conductivity));
    }
    else
    {
        ctd_ptr->set_conductivity(-1);
    }
    return space_pos+1;
}

int parse_double_field(const std::string& line, double* dbl_ptr)
{
    std::string::size_type space_pos = line.find(' ');
    if(space_pos != std::string::npos)
    {        
        std::string str_field = line.substr(0, space_pos);
        *dbl_ptr = std::stod(str_field);
    }
    else
    {
        *dbl_ptr = std::numeric_limits<double>::quiet_NaN();
    }
    return space_pos + 1;
}

    
int main(int argc, char* argv[])
{
    show_vector();

    double temp = 4.123456789;
    std::cout << "High precision (default 6): " << temp << std::endl;
    std::cout << "High precision (full precision): " << std::setprecision(std::numeric_limits<double>::digits10) << temp << std::endl;
    std::cout << "Sig digs in double: " << std::numeric_limits<double>::digits10 << std::endl;
    std::cout << "Sig digs in float: " << std::numeric_limits<float>::digits10 << std::endl;

    std::cout << "Integer max: " << std::numeric_limits<int>::max() << std::endl;
    std::cout << "Long max: " << std::numeric_limits<long>::max() << std::endl;   
    
    
    
    std::string line;
    
    while(std::getline(std::cin, line))
    {
        std::cout << line << std::endl;
        
        std::cout << "line was " << line.size() << " characters." << std::endl;

        if(line.empty())
            continue;

        protobuf::CTDSample ctd_sample;

        {
            int next_index = parse_conductivity(line, &ctd_sample);
            line = line.substr(next_index);
            //   std::cout << "C: " << ctd_sample.conductivity() << std::endl;
        }
        
        {
            double pressure;
            int next_index = parse_double_field(line, &pressure);
            ctd_sample.set_pressure(pressure);
            line = line.substr(next_index);
            ///std::cout << "P: " << std::setprecision(std::numeric_limits<double>::digits10) << ctd_sample.pressure() << std::endl;
        }

        {
            double temperature;
            int next_index = parse_double_field(line, &temperature);
            ctd_sample.set_temperature(temperature);
            line = line.substr(next_index);
            //std::cout << "T: " << std::setprecision(std::numeric_limits<double>::digits10) << ctd_sample.temperature() << std::endl;
        }

        std::cout << ctd_sample.ShortDebugString() << std::endl;

        std::vector<unsigned char> bytes(ctd_sample.ByteSize());
        ctd_sample.SerializeToArray(&bytes[0], bytes.size());

        std::cout << "Protobuf encoded length: " << ctd_sample.ByteSize() << std::endl;
        std::cout << "ASCII length: " << ctd_sample.ShortDebugString().size() << std::endl;
        
        for(unsigned char b : bytes)
            std::cout << std::hex << static_cast<unsigned int>(b);
        std::cout << std::endl;

        // receiver gets bytes
        protobuf::CTDSample ctd_sample_input;
        ctd_sample_input.ParseFromArray(&bytes[0], bytes.size());

        std::cout << "Parsed protobuf: " << ctd_sample_input.ShortDebugString() << std::endl;

        dccl::Codec dccl;
        dccl.load<protobuf::CTDSample>();
        
        std::vector<char> dccl_bytes(dccl.size(ctd_sample));
        dccl.encode(&dccl_bytes[0], dccl_bytes.size(), ctd_sample);

        std::cout << "DCCL encoded length: " << dccl.size(ctd_sample) << std::endl;
        for(char b : dccl_bytes)
            std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(b));
        std::cout << std::endl;
        
        
    }   
}
