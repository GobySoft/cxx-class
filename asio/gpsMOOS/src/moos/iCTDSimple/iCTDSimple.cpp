#include "iCTDSimple.h"

using goby::glog;
using namespace goby::common::logger;
using goby::moos::operator<<;

boost::shared_ptr<CTDSimpleConfig> master_config;
CTDSimple* CTDSimple::inst_ = 0;


 
int main(int argc, char* argv[])
{
    return goby::moos::run<CTDSimple>(argc, argv);
}


CTDSimple* CTDSimple::get_instance()
{
    if(!inst_)
    {
        master_config.reset(new CTDSimpleConfig);
        inst_ = new CTDSimple(*master_config);
    }
    return inst_;
}

void CTDSimple::delete_instance()
{
    delete inst_;
}


CTDSimple::CTDSimple(CTDSimpleConfig& cfg)
    : GobyMOOSApp(&cfg),
      cfg_(cfg),
      serial_(cfg_.serial_port(), cfg_.serial_baud())
{
    serial_.start();
    serial_.write("MONITOR\r");
}


CTDSimple::~CTDSimple()
{
    serial_.close();
}



void CTDSimple::loop()
{
    std::string line;
    while(serial_.readline(&line))
    {        
        glog.is(DEBUG1) && glog << line << std::endl;
        
        if(line.empty())
            continue;

        protobuf::CTDSample ctd_sample;
        {
            int next_index = parse_conductivity(line, &ctd_sample);
            line = line.substr(next_index);
        }
        
        {
            double pressure;
            int next_index = parse_double_field(line, &pressure);
            ctd_sample.set_pressure(pressure);
            line = line.substr(next_index);
        }

        {
            double temperature;
            int next_index = parse_double_field(line, &temperature);
            ctd_sample.set_temperature(temperature);
            line = line.substr(next_index);
        }

        glog.is(DEBUG1) && glog  << ctd_sample.ShortDebugString() << std::endl;

        publish_pb("CTD_SAMPLE", ctd_sample);
    }
}

int CTDSimple::parse_conductivity(const std::string& line, protobuf::CTDSample* ctd_ptr)
{
    std::string::size_type space_pos = line.find(' ');
    if(space_pos != std::string::npos)
    {        
        std::string str_conductivity = line.substr(0, space_pos);
        ctd_ptr->set_conductivity(std::stoi(str_conductivity));
    }
    else
    {
        ctd_ptr->set_conductivity(-1);
    }
    return space_pos+1;
}

int CTDSimple::parse_double_field(const std::string& line, double* dbl_ptr)
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
