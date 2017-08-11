// The basic server class that will be run at all points along the link.

#include <boost/asio.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "iUDPDroneReceiver.h"
#include "iUDPDroneReceiver_config.pb.h"
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

/**** Members necessary to be a MOOS app ****/
// setting up singleton for MOOSApp use
boost::shared_ptr<multihop::UDPDroneReceiverConfig> master_config;
UDPDroneReceiver* UDPDroneReceiver::inst_ = 0;

UDPDroneReceiver* UDPDroneReceiver::get_instance()
{
  if (!inst_)
    {
      master_config.reset(new multihop::UDPDroneReceiverConfig);
      inst_ = new UDPDroneReceiver(*master_config);
    }
  return inst_;
}

void UDPDroneReceiver::delete_instance()
{
  delete inst_;
}

// Will loop through poll, waiting for some other class to call send_data and pass it a UDPMessage.
// For now, it also prints out the contents of any protobuf message it receives, but eventually it
// will deal with things it receives by publishing them to the MOOSDB. So at present it's basically
// the jetyak/"receive mode" of the old main loop.
void UDPDroneReceiver::loop()
{
  io_service.poll();
  
  if (!messages.empty()) {
    std::string pingresult;
    if (messages.front().dest()) {
      pingresult = exec("~/Documents/August9/cxx-class/asio/gpsMOOS/ping_topside.sh");
    } else {
      pingresult = exec("~/Documents/August9/cxx-class/asio/gpsMOOS/ping_jetyak.sh");
    }
    if (pingresult.size() && stoi(pingresult.substr(pingresult.find(" packets transmitted, ")+22,1))) {
      publish_pb("UDP_MESSAGE_IN", messages.front());
      messages.pop();
    }
  }
  
}
/**** end MOOSApp-specific member functions ****/

// Formerly took a long list of parameters:
  // An io_service.
  // Its position within the multi-hop comms chain (a short).
  // The port it was using on this machine (a short).
  // The IP (boost::asio::ip::address) and port (short) on the next and previous machines (in that
  // order).
  // Hopefully some or all of these will be replaced by the iSerialConfig.
UDPDroneReceiver::UDPDroneReceiver(multihop::UDPDroneReceiverConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    socket_(io_service, udp::endpoint(udp::v4(),cfg_.port()))
      
  {

    int arraysize = cfg_.load_protobuf_shared_lib_size();
    for (int i = 0 ; i < arraysize ; i++)
      {
	goby::util::DynamicProtobufManager::load_from_shared_lib(cfg_.load_protobuf_shared_lib(i));
      }

    do_receive();
  }

void UDPDroneReceiver::do_receive()
  {
    auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
          {
            if (!ec && bytes_recvd > 0 /*&& from previous link */)
              {
		data_ = *(new std::string(cstr_data_, bytes_recvd));
		msg.ParseFromString(data_);
		messages.push(msg);
                do_receive();
              }

            else
              {
                do_receive();
              }
          };
       
    // the program listens on its designated port
    socket_.async_receive_from(boost::asio::buffer(cstr_data_, max_length), receive_port_, receive_handler);

  }

int main(int argc, char* argv[])
{

  return goby::moos::run<UDPDroneReceiver>(argc,argv);

}

    // IP addresses for reference:
    // Benthophilina (RasPi): 192.168.143.135
    // Catalina (Jonathan's laptop): 192.168.143.108
    // Neon (Lauren's laptop): 192.168.143.121
