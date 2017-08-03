// The basic server class that will be run at all points along the link.

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "messages/gps.pb.h"
#include "iUDPServer.h"
#include "goby/util/binary.h"
#include "iUDPServer_config.pb.h" // Namespace: multihop
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

/**** Members necessary to be a MOOS app ****/
// setting up singleton for MOOSApp use
boost::shared_ptr<multihop::UDPServerConfig> master_config;
UDPServer* UDPServer::inst_ = 0;

UDPServer* UDPServer::get_instance()
{
  if (!inst_)
    {
      master_config.reset(new multihop::UDPServerConfig);
      inst_ = new UDPServer(*master_config);
    }
  return inst_;
}

void UDPServer::delete_instance()
{
  delete inst_;
}

// Will loop through poll, waiting for some other class to call send_data and pass it a UDPMessage.
// For now, it also prints out the contents of any protobuf message it receives, but eventually it
// will deal with things it receives by publishing them to the MOOSDB. So at present it's basically
// the jetyak/"receive mode" of the old main loop.
void UDPServer::loop()
{
  io_service.poll();
  
  if (isnew) {
    
    multihop::UDPMessage msg1;
    boost::shared_ptr<google::protobuf::Message> msg2;
    msg1.ParseFromString(data_);
    (*msg2).ParseFromString(msg1.serialized());	
    
    std::cout << msg2->ShortDebugString() << std::endl;
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
UDPServer::UDPServer(multihop::UDPServerConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    position(cfg_.position()),
    socket_(io_service, udp::endpoint(udp::v4(),cfg_.port()))
      
  {
    
    // initialize the endpoints of neighboring comms links
    next_link_.address(boost::asio::ip::address::from_string(cfg_.nextip())); 
    next_link_.port(cfg_.nextport());
    prev_link_.address(boost::asio::ip::address::from_string(cfg_.previp())); 
    prev_link_.port(cfg_.prevport());

    isnew = 0; // Assume there's no new data to read to start out with.

    do_receive();  //normally should trigger on do_recieve; changed for testing

    subscribe_pb("UDP_MESSAGE", &UDPServer::handle_udp_message, this);
  }

void UDPServer::do_receive()
  {
    auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
          {
            if (!ec && bytes_recvd > 0 /*&& from previous link */)
              {
		data_ = *(new std::string(cstr_data_, bytes_recvd));
		msg.ParseFromString(data_);
		if (msg.destination()>position) { do_send_forward(data_); }
		else if (msg.destination()<position) { do_send_back(data_); }
		else { isnew = 1; } // Eventually, I think that rather than
                                    // juggling isnew, we'll have another class
                                    // to interface between UDPServer and the
                                    // MOOSDB. So at this point in the code,
                                    // the UDPMessage would be passed to it.
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

std::string UDPServer::get_str()
{
  isnew = 0;
  return data_;
}

// Keeps isnew private while allowing outside functions like main to not get the same data forever
bool UDPServer::hasnew()
  {
    return isnew;
  }
  
    //send to the next link forward (shore -> drone -> jetyak -> auv)
  // This and do_send_back are used when the UDPServer receives a UDP packet from the chain and passes
  // it directly on to a neighboring machine. To send data from this machine, send_data is used.
void UDPServer::do_send_forward(std::string tosend)
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

    socket_.async_send_to(boost::asio::buffer(tosend, tosend.size()), next_link_, send_handler);
  }

  

  //send to the next link back (shore -> drone -> jetyak -> auv)
void UDPServer::do_send_back(std::string tosend)
  {
       auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

       socket_.async_send_to(boost::asio::buffer(tosend, tosend.size()), prev_link_, send_handler);
  }

// Receiving outbound messages from the MOOSDB.
void UDPServer::handle_udp_message(const multihop::UDPMessage& msg)
{
    auto send_handler = [this](boost::system::error_code ec, std::size_t bytes_sent)
        {
	  do_receive();
        };

    multihop::UDPMessage msg_copy(msg); // to get around "const" problems

    std::cout << "Sending (theoretically): " << msg_copy.ShortDebugString() << std::endl;
    
    msg_copy.set_source(position);
    std::string msg_str;
    msg_copy.SerializeToString(&msg_str);
    if (msg_copy.destination()>position) { do_send_forward(msg_str); }
    else if (msg_copy.destination()<position) { do_send_back(msg_str); }
}

int main(int argc, char* argv[])
{

  return goby::moos::run<UDPServer>(argc,argv);

}

    // IP addresses for reference:
    // Benthophilina (RasPi): 192.168.143.135
    // Catalina (Jonathan's laptop): 192.168.143.108
    // Neon (Lauren's laptop): 192.168.143.121
