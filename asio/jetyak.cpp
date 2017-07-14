
// to talk to a UDP port via command line: socat - udp-sendto:127.0.0.1:<port #>
// to listen to a UDP port via command line: netcat -ul <port#>



#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include <sstream>

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

  void do_receive()
  {
    auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
          {
	    if (!ec && bytes_recvd > 0 /*&& from previous link */)
              {

		unsigned char* test_data = boost::asio::buffer_cast<unsigned char*>(boost::asio::buffer(data_,max_length));
		incoming_data_ = test_data;		
              }

	       	
	    // if (!ec && bytes_recvd > 0 /*&& from future link */)
	    //   {
	    // 	do_send_back(bytes_recvd);
	    // 	incoming_data_ = bytes_recvd;
	    //   }
	    else
              {
		do_receive();
              }
          };
       
      //recieve is only on one port, this computer's port
    socket_.async_receive_from(boost::asio::buffer(data_, max_length), recieve_port_, receive_handler);

  }

  //send to the next link forward (shore -> drone -> jetyak -> auv)
  void do_send_forward(std::size_t length) // really no idea why this is taking a length argument?
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

    //address of next link in the chain
    next_link_.address(boost::asio::ip::address::from_string("127.0.0.1")); 
    next_link_.port(50004); // choose a port, any port

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
       prev_link_.port(50005); //choose a port, any port

       socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

  unsigned char* get_data()
  {
    return incoming_data_;  
  }
  

private:
  udp::socket socket_;
  udp::endpoint recieve_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  unsigned char* incoming_data_;
  std::string incoming_data_str_;
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

      // standard stringstream doesn't work here! Something about null pointers remaining at the end? I'm not sure.
      unsigned char* input = s.get_data();
      std::ostringstream ss;
      ss << input;
      std::string input_string = ss.str();

      protobuf::GPSPosition gps;
      io_service.poll();
      //   std::cout << "GPS string \n " << gps.ShortDebugString()  << std::endl;
      

      gps.ParseFromString(input_string); 
      
      std::cout << gps.ShortDebugString() << std::endl;
	
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
