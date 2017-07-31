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
    { 
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
  int arraysize = cfg_.array_size();
  for (int i = 0 ; i < arraysize ; i++)
    {
      subscribe(cfg_.array(i).moos_var(), &Packager::handle_pb_message, this);
    }
}


Packager::~Packager()
{
    
}

void Packager::loop()
{
}

// to be included as a private member function of Packager
int Packager::mapLAMSSDest(int dest)
{
  switch (dest)
    {
    case 1 : return 0;
    // 1 is reserved for the drone
    case 10 : return 2;
    case 5 : return 3;
    }
  std::cerr << "Invalid dest (in function mapLAMSSDest)" << std::endl;
  return -1;
}

void Packager::handle_pb_message(const CMOOSMsg& cmsg)
{

  // turn subscribed CMOOSMsg into smart pointer to generic protobuf
  boost::shared_ptr<google::protobuf::Message> msg_ptr = dynamic_parse_for_moos(cmsg.GetString());

  // prepare to loop through protobufs given in mission/config file
  int arraysize = cfg_.array_size();

  // get name of protobuf being introspected
  const std::string& name = msg_ptr->GetDescriptor()->full_name();

  // initialize FieldDescriptor for dest field of protobuf
  const google::protobuf::FieldDescriptor* destFD;

  // loop through the known protobuf types given in the mission file, and when
  // a name that matches is found, use the dest_field name it gives to get the
  // FieldDescriptor for destFD
  for (int i = 0 ; i < arraysize ; i++)
    {
      if (cfg_.array(i).class_name()==name) {
	destFD = msg_ptr->GetDescriptor()->FindFieldByName(cfg_.array(i).dest_field());
      }
    }

  // use the dereferencing of msg_ptr and a pointer to destFD to determine what
  // type is stored in the dest field of *msg_ptr, and retrieve it
  int LAMSSdest;
  switch(destFD->cpp_type())
    {
    case 1 : LAMSSdest = msg_ptr->GetReflection()->GetInt32(*msg_ptr, destFD);
      break;
    case 2 : LAMSSdest = msg_ptr->GetReflection()->GetInt64(*msg_ptr, destFD);
      break;
    case 3 : LAMSSdest = msg_ptr->GetReflection()->GetUInt32(*msg_ptr, destFD);
      break;
    case 4 : LAMSSdest = msg_ptr->GetReflection()->GetUInt64(*msg_ptr, destFD);
      break;
    case 5 :
    case 6 :
    case 7 :
    case 8 :
      std::cerr << "Invalid dest type (should be int, unsigned int, or string)." << std::endl;
      break;
    case 9 : LAMSSdest = std::atoi(msg_ptr->GetReflection()->GetString(*msg_ptr, destFD).c_str());
    case 10 :
      std::cerr << "Invalid dest type (should be int, unsigned int, or string)." << std::endl;
      break;
    }

  // convert LAMSS numbering to numbering for multi-hop chain
  int UDPdest = mapLAMSSDest(LAMSSdest);
  
  /* Assembling the UDPMessage to be sent out. */
  udp_proto::UDPMessage udp;
  udp.set_source(-1); // Will be set for real when iServer sends it. pPackager does not know
                      // about the specific machine it's running on.
  udp.set_destination(UDPdest);
  std::string msg_string;
  msg_ptr->SerializeToString(&msg_string);
  udp.set_serialized(msg_string);
  
  /* Publishing the UDPMessage to the MOOSDB */
  publish_pb("UDP_MESSAGE", udp);
}
