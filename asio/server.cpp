
// to talk to a UDP port via command line: socat - udp-sendto:127.0.0.1:<port #>
// to listen to a UDP port via command line: netcat -ul <port#>



#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "udp.pb.h"
#include "gps.pb.h"
#include "server.h"

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

  // Can be called when there are outgoing messages to be sent from this machine.
void server::send_data(std::string msg)
  {
    auto send_handler = [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        };

    socket_.async_send_to(boost::asio::buffer(msg,max_length), prev_link_, send_handler);
  }

char* server::get_data()
  {
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
    // Next block excised because ports and IP addresses are now hardcoded into main.
/* if (argc != 6)
    {
      std::cerr << "Usage: jetyak <this port> <IP address of next machine in chain> <port on that machine> <IP address of previous machine in chain> <port on that machine>\n";
      return 1;
    }
*/

    boost::asio::io_service io_service;

    // Currently set to send to Benthophilina.
    // IP addresses:
    // Benthophilina (RasPi): 192.168.143.135
    // Catalina (Jonathan's laptop): 192.168.143.108
    // Neon (Lauren's laptop): 192.168.143.121
    server s(io_service, 1, 5000, boost::asio::ip::address::from_string("192.168.143.135"), 5000, boost::asio::ip::address::from_string("192.168.143.135"), 5000 );

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

      gps::GPSMessage msg;
      io_service.poll();
      //   std::cout << "GPS string \n " << gps.ShortDebugString()  << std::endl;
      
      if (s.hasnew()) {
	msg.ParseFromString(input_string); 
      
	std::cout << msg.ShortDebugString() << std::endl;
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
