#include "pUnpackager.h"
#include "messages/udp.pb.h"
#include "pUnpackager_config.pb.h"
#include "goby/moos/goby_moos_app.h"


using namespace goby::common::logger;

boost::shared_ptr<multihop::UnpackagerConfig> master_config;
Unpackager* Unpackager::inst_ = 0;

 
int main(int argc, char* argv[])
{
    return goby::moos::run<Unpackager>(argc, argv);
}

// Specific to GobyMOOSApp: instantiates Unpackager if it doesn't already exist
Unpackager* Unpackager::get_instance()
{
  if(!inst_)
    { 
      master_config.reset(new multihop::UnpackagerConfig);
      inst_ = new Unpackager(*master_config);
    }
    return inst_;
}

// Specific to GobyMOOSApp: deinstantiates Unpackager
void Unpackager::delete_instance()
{
    delete inst_;
}

// (private) constructor: loops through array of FieldSpecifics and subscribes to each protobuf
Unpackager::Unpackager(multihop::UnpackagerConfig& cfg)
    : GobyMOOSApp(&cfg),
    cfg_(cfg)
{
  int arraysize = cfg_.load_protobuf_shared_lib_size();
  for (int i = 0 ; i < arraysize ; i++)
    {
    goby::util::DynamicProtobufManager::load_from_shared_lib(cfg_.load_protobuf_shared_lib(i));
    }

  subscribe_pb("UDP_MESSAGE_IN", &Unpackager::handle_udp_message, this);
}


Unpackager::~Unpackager()
{
}

void Unpackager::loop()
{
}

// Does not as of yet do anything with src or dest. This could imply that src will never be used and
// so should be removed.

void Unpackager::handle_udp_message(const multihop::UDPMessage& msg)
{
  boost::shared_ptr<google::protobuf::Message> msg_ptr(goby::util::DynamicProtobufManager::new_protobuf_message(msg.protobuf_type()));
  msg_ptr->ParseFromString(msg.serialized());

  // prepare to loop through protobufs given in mission/config file
  int arraysize = cfg_.array_size();

  std::string moos_var;

  // loop through the known protobuf types given in the mission file, and when
  // a name that matches is found, use the moos_var name it gives to get the
  // FieldDescriptor for moosFD
  for (int i = 0 ; i < arraysize ; i++)
    {
      if (cfg_.array(i).class_name()==msg.protobuf_type()) {
	// get moos_var from protobuf
	moos_var = cfg_.array(i).moos_var();
      }
    }

  publish_pb(moos_var, (*msg_ptr));
}
