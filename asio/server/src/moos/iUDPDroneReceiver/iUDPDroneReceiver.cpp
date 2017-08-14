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
      pingresult = exec("/home/jonathan/server/ping_jetyak_office.sh");
    } else {
      pingresult = exec("/home/jonathan/server/ping_topside_office.sh");
    }
    if (pingresult.size() && stoi(pingresult.substr(pingresult.find(" packets transmitted, ")+22,1))) {
      publish_pb("UDP_MESSAGE", messages.front());
      messages.pop();
    }
  }
  
}
/**** end MOOSApp-specific member functions ****/

UDPDroneReceiver::UDPDroneReceiver(multihop::UDPDroneReceiverConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    socket_(io_service, udp::endpoint(udp::v4(),cfg_.port()))
      
  {
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
