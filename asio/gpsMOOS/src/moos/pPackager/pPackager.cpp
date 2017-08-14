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

// Specific to GobyMOOSApp: deinstantiates Packager
void Packager::delete_instance()
{
    delete inst_;
}

// (private) constructor: loops through array of FieldSpecifics and subscribes to each protobuf
Packager::Packager(multihop::PackagerConfig& cfg)
    : GobyMOOSApp(&cfg),
      cfg_(cfg),
      udp()
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

// Function that executes a shell script.


// Publishes UDP for transmission, if and only if the Edison is connected.
void Packager::loop()
{
  std::string pingresult = exec("~/cxx-class/asio/gpsMOOS/ping_edison_office.sh");

  // Should determine whether or not packet has been received by extracting the one-character substring following the phrase "packts transmitted" after a single ping, and executing if that substring is not 0.
  if (pingresult.size()) {
    /*DEBUG*/std::cout << "Substring?" << std::endl;
    /*DEBUG*/std::cout << pingresult << std::endl;

    if (stoi(pingresult.substr(pingresult.find(" packets transmitted, ")+22,1)))
      {
	/*DEBUG*/std::cout << "Substring." << std::endl;
	if (udp.msg_size()) {
	  /*DEBUG*/ std::cout << "ping recieved, ready to publish udp to moos" << std::endl;
	  udp.set_dest(-1); // Will be set for real by the iUDPServer.
	  publish_pb("UDP_MESSAGE_OUT", udp);
	  /*DEBUG*/ std::cout << "UDP Message published" << std::endl;
	  multihop::UDPMessage new_udp;
	  udp.Clear(); // Is there a better way to refresh udp?
	}
      }
    /*DEBUG*/std::cout << "Substring. (End if.)" << std::endl;
  }
  
}

void Packager::handle_pb_message(const CMOOSMsg& cmsg)
{
  // turn subscribed CMOOSMsg into smart pointer to generic protobuf
  boost::shared_ptr<google::protobuf::Message> msg_ptr = dynamic_parse_for_moos(cmsg.GetString());

  // get name of protobuf being introspected
  std::string name = msg_ptr->GetDescriptor()->full_name();

  multihop::UDPMessage::serialized msg_ser;
  msg_ser.set_protobuf_type(name);

  int arraysize = cfg_.array_size();
  std::string moos_var;
  for (int i = 0 ; i < arraysize ; i++)
    {
      if (cfg_.array(i).class_name()==name) {
	moos_var = cfg_.array(i).moos_var();
      }
    }
  msg_ser.set_moos_var(moos_var);

  std::string msg_string;
  msg_ptr->SerializeToString(&msg_string);
  msg_ser.set_data(msg_string);

  *udp.add_msg() = msg_ser;
}
