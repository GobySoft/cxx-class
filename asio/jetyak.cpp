
// to talk to a UDP port via command line: socat - udp-sendto:127.0.0.1:<port #>
// to listen to a UDP port via command line: netcat -ul <port#>



#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
//sstream?

#include "udp.pb.h"

using boost::asio::ip::udp;

class server
{
public:
  server(boost::asio::io_service& io_service, short port, std::string nextIP, short nextport, std::string prevIP, short prevport)
    : socket_(io_service, udp::endpoint(udp::v4(), port))
      
  {
    // initialize the endpoints of neighboring comms links
    next_link_.address(boost::asio::ip::address::from_string(nextIP)); 
    next_link_.port(nextport);
    prev_link_.address(boost::asio::ip::address::from_string(prevIP)); 
    prev_link_.port(prevport);

    isnew = 0; // Assume there's no new data to read to start out with.

    do_receive();  //normally should trigger on do_recieve; changed for testing
  }

  void do_receive()
  {
    auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
          {
	    if (!ec && bytes_recvd > 0 /*&& from previous link */)
              {

		//		data_ = boost::asio::buffer_cast<char[1024]>(boost::asio::buffer(data_,max_length));
		isnew = 1;
		do_receive();
              }

	    else
              {
		isnew = 0;
		do_receive();
              }
          };
       
      //receive is only on one port, this program's port
    socket_.async_receive_from(boost::asio::buffer(data_, max_length), receive_port_, receive_handler);

  }

  //send to the next link forward (shore -> drone -> jetyak -> auv)
  void do_send_forward(std::size_t length) // really no idea why this is taking a length argument?
                                           // Probably to specify the amount of data being sent.
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

    //address of next link in the chain



      socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

  

  //send to the next link back (shore -> drone -> jetyak -> auv) nnn
  void do_send_back(std::size_t length)
  {
       auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

       // Also, for this and send_forward: is it necessary to set address and port for every
       // send action, or could that be dealt with back in the constructor?

       socket_.async_send_to(boost::asio::buffer(data_, length), next_link_, send_handler);
  }

  char* get_data()
  {
    return data_;  
  }

  // Keeps isnew private while allowing outside functions like main to not get the same data forever
  bool hasnew()
  {
    return isnew;
  }
  

private:
  udp::socket socket_;
  udp::endpoint receive_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum { max_length = 1024 };
  char data_[max_length];
  bool isnew;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 6)
    {
      std::cerr << "Usage: jetyak <this port> <IP address of next machine in chain> <port on that machine> <IP address of previous machine in chain> <port on that machine>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    
    server s(io_service, std::atoi(argv[1]), std::string(argv[2]), std::atoi(argv[3]), std::string(argv[4]), std::atoi(argv[5]));

    // in loop method of GobyMOOSApp
    while (1)
    {

      // standard stringstream doesn't work here! Something about null pointers remaining at the end? I'm not sure.
      //      unsigned char* input = s.get_data();
      //      std::ostringstream ss; // FLAG: What?
      //      ss << input;
      //      std::string input_string = ss.str(); // Are these three lines just a conversion of input to
                                           // std::string?

      // If this doesn't work, look at four lines above.
      // (Constructs std string from C-style string held in get_data().)
      std::string input_string(s.get_data());

      protobuf::GPSPosition gps;
      io_service.poll();
      //   std::cout << "GPS string \n " << gps.ShortDebugString()  << std::endl;
      
      if (s.hasnew()) {
	gps.ParseFromString(input_string); 
      
	std::cout << gps.ShortDebugString() << std::endl;
      }
	
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
