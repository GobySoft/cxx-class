
// to talk to a UDP port via command line: socat - udp-sendto:127.0.0.1:<port #>
// to listen to a UDP port via command line: netcat -ul <port#>

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "gpsreader.h"
#include "dccl.h"
#include "udp.pb.h"

using boost::asio::ip::udp;

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : socket_(io_service, udp::endpoint(udp::v4(), port))
      
  {
    do_receive();  //normally should trigger on do_recieve; changed for testing
  }

  void send_data( std::string msg/*protobuf::GPSPosition gps*/)
  {
      auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

       prev_link_.address(boost::asio::ip::address::from_string("127.0.0.1")); 
       prev_link_.port(50002); //choose a port, any port

       // this works too, I ended up using strings because I was got confused while chasing a different bug, but this is fine
       // boost::asio::streambuf b; 
       // std::ostream os(&b);
       // gps.SerializeToOstream(&os);
       
       // socket_.async_send_to(b.data(), prev_link_, send_handler);

       
       socket_.async_send_to(boost::asio::buffer(msg,max_length), prev_link_,send_handler);
  }
  
  void do_receive()
  {
      auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd)
          {
	    if (!ec && bytes_recvd > 0 /* && from previous link */)
              {
		do_send_forward(bytes_recvd);
              } 
	    if (!ec && bytes_recvd > 0 /*&& from future link */)
	      {
	    	do_send_back(bytes_recvd);
	    }
	    else
              {
		do_receive();
              }
          };
       
      //recieve is only on one port, this computer's port
    socket_.async_receive_from(boost::asio::buffer(data_, max_length), recieve_port_, receive_handler);
  }

  //send to the next link forward (shore -> drone -> jetyak -> auv)
  void do_send_forward(std::size_t length) 
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

    //address of next link in the chain
    next_link_.address(boost::asio::ip::address::from_string("127.0.0.1")); 
    next_link_.port(50001); // choose a port, any port

      socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

  //send to the next link back (shore -> drone -> jetyak -> auv) 
  void do_send_back(std::size_t length)
  {
       auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

       prev_link_.address(boost::asio::ip::address::from_string("127.0.0.1")); 
       prev_link_.port(50003); //choose a port, any port

       
       // protobuf::GPSPosition gps;
       // GPSPosition gps_proto;
       // gps_proto.set_proto(&gps);
      
       // boost::asio::streambuf b;
       // std::ostream os(&b);
       // gps.SerializeToOstream(&os);
      
       // socket_.async_send_to(b.data(), prev_link_, send_handler);

       
       // boost::asio::async_write(socket_,b,send_handler);
       
       // std::string msg = gps.ShortDebugString();
       //socket_.async_send_to(b.data(), prev_link_, send_handler);
  }

private:
  udp::socket socket_;
  udp::endpoint recieve_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum { max_length = 1024 };
  char data_[max_length];
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_udp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    
    server s(io_service, std::atoi(argv[1]));

    
    // in loop method of GobyMOOSApp
    while (1)
    {
      
      std::string line;
      std::vector<std::unique_ptr<GPSPosition>>gps_positions;
      
      
      // while(std::getline(std::cin, line))
      // 	{
      // 	  enum { TALKER_LENGTH = 6 };
      // 	  if(line.size() > TALKER_LENGTH)
      // 	    {
      // 	      auto talker = line.substr(3,3);
      // 	      if(talker == "GGA")
      // 		gps_positions.push_back(std::unique_ptr<GPSPosition>(new GGASentence(line)));
      // 	      else if(talker == "RMC")
      // 		gps_positions.push_back(std::unique_ptr<GPSPosition>(new RMCSentence(line)));
      // 	    }
      // 	}

       protobuf::GPSPosition gps;
       // GPSPosition gps_proto;
       // gps_proto.set_proto(&gps);
        gps.set_time(45);
        gps.set_longitude(41.789435);
        gps.set_latitude(71.45672);

	std::string gps_msg;
       gps.SerializeToString(&gps_msg);
       
	s.send_data(gps_msg);
	
        io_service.poll();
        //...other work
        usleep(1000);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
