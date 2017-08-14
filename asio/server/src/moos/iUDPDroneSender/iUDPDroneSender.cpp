// The basic server class that will be run at all points along the link.

#include <boost/asio.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "iUDPDroneSender.h"
#include "iUDPDroneSender_config.pb.h"
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

/**** Members necessary to be a MOOS app ****/
// setting up singleton for MOOSApp use
boost::shared_ptr<multihop::UDPDroneSenderConfig> master_config;
UDPDroneSender* UDPDroneSender::inst_ = 0;

UDPDroneSender* UDPDroneSender::get_instance()
{
  if (!inst_)
    {
      master_config.reset(new multihop::UDPDroneSenderConfig);
      inst_ = new UDPDroneSender(*master_config);
    }
  return inst_;
}

void UDPDroneSender::delete_instance()
{
  delete inst_;
}

// Will loop through poll, waiting for some other class to call send_data and pass it a UDPMessage.
// For now, it also prints out the contents of any protobuf message it receives, but eventually it
// will deal with things it receives by publishing them to the MOOSDB. So at present it's basically
// the jetyak/"receive mode" of the old main loop.
void UDPDroneSender::loop()
{
  io_service.poll();
  
}
/**** end MOOSApp-specific member functions ****/

UDPDroneSender::UDPDroneSender(multihop::UDPDroneSenderConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    socket_(io_service, udp::endpoint(udp::v4(),cfg_.port()))

{

    // initialize the endpoints of neighboring comms links
    next_link_.address(boost::asio::ip::address::from_string(cfg_.nextip())); 
    next_link_.port(cfg_.nextport());
    prev_link_.address(boost::asio::ip::address::from_string(cfg_.previp())); 
    prev_link_.port(cfg_.prevport());

    subscribe_pb("UDP_MESSAGE", &UDPDroneSender::handle_udp_message, this);
}

    //send to the next link forward (shore -> drone -> jetyak)
    // This and do_send_back are used when the UDPDroneSender receives a UDP packet from the chain and passes
    // it directly on to a neighboring machine. To send data from this machine, send_data is used.
void UDPDroneSender::do_send_forward(std::string tosend)
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {};
    socket_.async_send_to(boost::asio::buffer(tosend, tosend.size()), next_link_, send_handler);
  }

  

  //send to the next link back (jetyak -> drone -> auv)
void UDPDroneSender::do_send_back(std::string tosend)
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {};
    socket_.async_send_to(boost::asio::buffer(tosend, tosend.size()), prev_link_, send_handler);
  }

// Receiving outbound messages from the iUDPDroneReceiver via the MOOSDB.
void UDPDroneSender::handle_udp_message(const multihop::UDPMessage& msg)
{
    std::string msg_str;
    
    // If dest is 0, sends back; if dest is 2, sends forward.
    if (msg.dest())
      {
	msg.SerializeToString(&msg_str);
	do_send_forward(msg_str);
      }
    else
      {
	msg.SerializeToString(&msg_str);
	do_send_forward(msg_str);
      }

}

int main(int argc, char* argv[])
{

  return goby::moos::run<UDPDroneSender>(argc,argv);

}

    // IP addresses for reference:
    // Benthophilina (RasPi): 192.168.143.135
    // Catalina (Jonathan's laptop): 192.168.143.108
    // Neon (Lauren's laptop): 192.168.143.121
