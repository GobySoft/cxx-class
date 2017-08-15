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
  // When there is data that can be sent:
  if (udp_contents.size()) {

      std::string pingresult = exec("~/Documents/August9/cxx-class/asio/gpsMOOS/ping_edison_office.sh");
      // Should determine whether or not packet has been received by extracting the one-character substring following the phrase "packets transmitted" after a single ping, and executing if that substring is not 0.
    if (pingresult.size())
      {

	if (stoi(pingresult.substr(pingresult.find(" packets transmitted, ")+22,1))) {

	  std::map<std::string,boost::shared_ptr<google::protobuf::Message>>::iterator it;
	  // Iterate across every message in udp_contents.
	  for ( it = udp_contents.begin();
		it != udp_contents.end();
		it++)
	    {
	      // Set the type label of serialized msg_ser for pUnpackager's use.
	      multihop::UDPMessage::serialized msg_ser;
	      msg_ser.set_protobuf_type(it->first);

	      // Loop across all protobuf types known from the config.
	      int arraysize = cfg_.array_size();
	      std::string moos_var;
	      for (int i = 0 ; i < arraysize ; i++)
		{
		  // When this particular protobuf is found, get its MOOS variable name.
		  if (cfg_.array(i).class_name()==it->first)
		    {
		      moos_var = cfg_.array(i).moos_var();
		    }
		}

	      // Label serialized msg_ser with the MOOS variable name.
	      msg_ser.set_moos_var(moos_var);

	      // Serialize the protobuf and add it to msg_ser.
	      std::string msg_string;
	      it->second->SerializeToString(&msg_string);
	      msg_ser.set_data(msg_string);

	      // Add the serialized protobuf to the outbound message.
	      *udp.add_msg() = msg_ser;
	      
	    }

	  // Set destination: a placeholder, but needed for publication.
	  udp.set_dest(-1); // Will be set for real by the iUDPServer.

	  // Publish the udp message so iUDPServer will send it.
	  publish_pb("UDP_MESSAGE_OUT", udp);

	  // Empty out udp and map for reuse.
	  udp.Clear();
	  udp_contents.clear();

	}
      }
  }
  
}

void Packager::handle_pb_message(const CMOOSMsg& cmsg)
{
  // turn subscribed CMOOSMsg into smart pointer to generic protobuf
  boost::shared_ptr<google::protobuf::Message> msg_ptr = dynamic_parse_for_moos(cmsg.GetString());

  // get name of protobuf being introspected
  std::string name = msg_ptr->GetDescriptor()->full_name();

  udp_contents.erase(name); // Will get rid of outdated message of same type.
                            // Not sure what happens if no such message present; probably nothing.
  udp_contents.insert(std::pair<std::string,boost::shared_ptr<google::protobuf::Message>>(name,msg_ptr)); // Adds new protobuf to map.

}
