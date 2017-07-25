// The basic server class that will be run at all points along the link.

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "udp.pb.h"
#include "gps.pb.h"
#include "server.h"
#include "goby/util/binary.h"

using boost::asio::ip::udp;


server::server(boost::asio::io_service& io_service, short pos, short port, boost::asio::ip::address nextIP, short nextport, boost::asio::ip::address prevIP, short prevport)
  : socket_(io_service, udp::endpoint(udp::v4(), port)),
    position(pos)
      
  {
    // initialize the endpoints of neighboring comms links
    next_link_.address(nextIP); 
    next_link_.port(nextport);
    prev_link_.address(prevIP); 
    prev_link_.port(prevport);

    isnew = 0; // Assume there's no new data to read to start out with.

    do_receive();  //normally should trigger on do_recieve; changed for testing
  }

void server::do_receive()
  {
    auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
          {
            if (!ec && bytes_recvd > 0 /*&& from previous link */)
              {
                isnew = 1;
		data_ = *(new std::string(cstr_data_, bytes_recvd));
                do_receive();
              }

            else
              {
                do_receive();
              }
          };
       
      //receive is only on one port, this program's port
    socket_.async_receive_from(boost::asio::buffer(cstr_data_, max_length), receive_port_, receive_handler);

  }

  // Can be called when there are outgoing messages to be sent from this machine.
void server::send_data(std::string msg)
  {
    auto send_handler = [this](boost::system::error_code ec, std::size_t bytes_sent)
        {
	  do_receive();
        };

	  socket_.async_send_to(boost::asio::buffer(msg,max_length), next_link_, send_handler);
  }

std::string server::get_str()
{
  isnew = 0;
  return data_;
}

// Keeps isnew private while allowing outside functions like main to not get the same data forever
bool server::hasnew()
  {
    return isnew;
  }
  
    //send to the next link forward (shore -> drone -> jetyak -> auv)
  // This and do_send_back are used when the server receives a UDP packet from the chain and passes
  // it directly on to a neighboring machine. To send data from this machine, send_data is used.
void server::do_send_forward(std::size_t length) // really no idea why this is taking a length argument?
                                           // Probably to specify the amount of data being sent.
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

      socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

  

  //send to the next link back (shore -> drone -> jetyak -> auv)
void server::do_send_back(std::size_t length)
  {
       auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

       // Also, for this and send_forward: is it necessary to set address and port for every
       // send action, or could that be dealt with back in the constructor?

       socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

int main(int argc, char* argv[])
{
  try
  {

    if (argc != 8)
      {
        std::cerr << "Usage: server <number of this program's position in the chain> <this port> <IP address of next machine in chain> <port on that machine> <IP address of previous machine in chain> <port on that machine> <binary value for send>\nex. server 1 5000 192.168.143.135 5000 127.0.0.1 5001 1\nTo clarify, that last thing is just in for debugging at the moment. If a 1 is given, this program will send the same hardcoded GPS message to the next machine in the link until the end of time; otherwise, it will just listen on the port given as the second argument.\n";
        return 1;
      }
    // IP addresses for reference:
    // Benthophilina (RasPi): 192.168.143.135
    // Catalina (Jonathan's laptop): 192.168.143.108
    // Neon (Lauren's laptop): 192.168.143.121

    
    boost::asio::io_service io_service;

    server s(io_service, std::atoi(argv[1]), std::atoi(argv[2]), boost::asio::ip::address::from_string(argv[3]), std::atoi(argv[4]), boost::asio::ip::address::from_string(argv[5]), std::atoi(argv[6]) );

    // in loop method of GobyMOOSApp
    if (std::atoi(argv[7])) {
      // If the program is told to send, it will execute this loop forever.
      // Formerly the main loop of drone.cpp.
      while (1)
	{
	  gps::GPSMessage msg2;
	  msg2.set_longitude(45.56789);
	  msg2.set_latitude(44.56789);
	  msg2.set_time(74);
	  
	  std::string gps_str;
	  msg2.SerializeToString(&gps_str);

	  udp_proto::UDPMessage msg1;
	  msg1.set_destination(1);
	  msg1.set_source(0);
	  msg1.set_serialized(gps_str);
	  
	  std::string msg_str;
	  msg1.SerializeToString(&msg_str);

	  s.send_data(msg_str);

	  
	  io_service.poll();
	  //...other work
	  usleep(1000000);
	}

    } else {
      // If the program is not told to send, it will execute this loop forever.
      while (1)
	{
	  
	  io_service.poll();
	  //   std::cout << "GPS string \n " << gps.ShortDebugString()  << std::endl;
      
	  if (s.hasnew()) {

	    std::string input_string(s.get_str());
	    udp_proto::UDPMessage msg1;
	    gps::GPSMessage msg2;
	    msg1.ParseFromString(input_string);
	    msg2.ParseFromString(msg1.serialized());	

	    std::cout << msg2.ShortDebugString() << std::endl;
	  }
	
	  //...other work
	  usleep(1000000);
	}
    }
  }
  
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
