#include "pPackager.h"
#include "messages/gps.pb.h"
#include "messages/udp.pb.h"
#include "pPackager_config.pb.h"


using namespace goby::common::logger;

boost::shared_ptr<gps_proto::PackagerConfig> master_config;
Packager* Packager::inst_ = 0;


 
int main(int argc, char* argv[])
{
    return goby::moos::run<Packager>(argc, argv);
}


Packager* Packager::get_instance()
{
  if(!inst_)
    { // somewhat baffling compiler complaint that master_config is an int
      master_config.reset(new gps_proto::PackagerConfig);
      inst_ = new Packager(*master_config);
    }
    return inst_;
}

void Packager::delete_instance()
{
    delete inst_;
}

Packager::Packager(gps_proto::PackagerConfig& cfg)
    : GobyMOOSApp(&cfg),
    cfg_(cfg)
{
  subscribe_pb("GPS_MESSAGE", &Packager::handle_gps_message, this);
}


Packager::~Packager()
{
    
}

void Packager::loop()
{
}

void Packager::handle_gps_message(const gps_proto::GPSMessage& gps)
{

  /* Assembling the UDPMessage to be sent out. */
  udp_proto::UDPMessage udp;
  udp.set_source(-1); // Will be set for real when iServer sends it. pPackager does not know
                      // about the specific machine it's running on.
  udp.set_destination(0); // For now, GPSMessages will be handed up the chain.
  std::string gps_string;
  gps.SerializeToString(&gps_string);
  udp.set_serialized(gps_string);
  
  /* Publishing the UDPMessage to the MOOSDB */
  publish_pb("UDP_MESSAGE", udp);
  std::cout << udp.ShortDebugString() << std::endl;
}
