#include "pGPSDisplayer.h"
#include "messages/gps.pb.h"
#include "pGPSDisplayer_config.pb.h"

using namespace goby::common::logger;

boost::shared_ptr<pGPSpb::GPSDisplayerConfig> master_config;
GPSDisplayer* GPSDisplayer::inst_ = 0;


 
int main(int argc, char* argv[])
{
    return goby::moos::run<GPSDisplayer>(argc, argv);
}


GPSDisplayer* GPSDisplayer::get_instance()
{
  if(!inst_)
    { // somewhat baffling compiler complaint that master_config is an int
      master_config.reset(new pGPSpb::GPSDisplayerConfig);
      inst_ = new GPSDisplayer(*master_config);
    }
    return inst_;
}

void GPSDisplayer::delete_instance()
{
    delete inst_;
}

// Equally baffling compiler complaint that it wants a constructor here.
GPSDisplayer::GPSDisplayer(pGPSpb::GPSDisplayerConfig& cfg)
    : GobyMOOSApp(&cfg),
    cfg_(cfg)
{
  subscribe_pb("GPS_MESSAGE", &GPSDisplayer::handle_gps_message, this);
}


GPSDisplayer::~GPSDisplayer()
{
    
}

void GPSDisplayer::loop()
{
}

void GPSDisplayer::handle_gps_message(const iGPSpb::GPSMessage& gps)
{
  std::cout << gps.ShortDebugString() << std::endl;
}

