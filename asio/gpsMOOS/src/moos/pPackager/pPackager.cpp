#include "pPackager.h"
#include "messages/udp.pb.h"
#include "pPackager_config.pb.h"
#include "goby/moos/goby_moos_app.h"


using namespace goby::common::logger;

boost::shared_ptr<multihop::PackagerConfig> master_config;
Packager* Packager::inst_ = 0;

 
int main(int argc, char* argv[])
{
    return goby::moos::run<Packager>(argc, argv);
}

// Specific to GobyMOOSApp: instantiates Packager if it doesn't already exist
Packager* Packager::get_instance()
{
  if(!inst_)
    { 
      master_config.reset(new multihop::PackagerConfig);
      inst_ = new Packager(*master_config);
    }
    return inst_;
}

// Specific to GobyMOOSApp: deinstantiaes Packager
void Packager::delete_instance()
{
    delete inst_;
}

// (private) constructor: loops through array of FieldSpecifics and subscribes to each protobuf
Packager::Packager(multihop::PackagerConfig& cfg)
    : GobyMOOSApp(&cfg),
    cfg_(cfg)
{
  int arraysize = cfg_.load_protobuf_shared_lib_size();
  for (int i = 0 ; i < arraysize ; i++)
    {
    goby::util::DynamicProtobufManager::load_from_shared_lib(cfg_.load_protobuf_shared_lib(i));
    }

  arraysize = cfg.array_size();
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

// maps known LAMSS device names onto the linear representation of the multi-hop comms system
int Packager::mapLAMSSDest(int dest)
{
  switch (dest)
    {
    case 1 : return 0; // topside
    // case ? : return 1 //1 is reserved for the drone
    case 10 : return 2; // jetyak
    case 5 : return 3; // SandShark
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
	                                   // get FieldDescriptor for dest_field from protobuf
	destFD = msg_ptr->GetDescriptor()->FindFieldByName(cfg_.array(i).dest_field());
      }
    }

  // use destFD and the dereferencing of msg_ptr to determine what
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
  multihop::UDPMessage udp;
  udp.set_source(-1); // Will be set for real when iServer sends it. pPackager does not know
                      // about the specific machine it's running on.
  udp.set_destination(UDPdest);
  std::string msg_string;
  msg_ptr->SerializeToString(&msg_string);
  udp.set_serialized(msg_string);

  /* Publishing the UDPMessage to the MOOSDB */
  publish_pb("UDP_MESSAGE", udp);

}
