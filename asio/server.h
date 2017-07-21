#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "udp.pb.h"

using boost::asio::ip::udp;

class server{
 public:
  server(boost::asio::io_service& io_service, short port, boost::asio::ip::address nextIP, short nextport, boost::asio::ip::address prevIP, short prevport);
  void do_receive();
  void send_data(std::string msg);
  char* get_data();
  bool hasnew();
 private:
  udp::socket socket_;
  udp::endpoint receive_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum {max_length = 1024};
  char data_[max_length];
  bool isnew;
  void do_send_forward(std::size_t length);
  void do_send_back(std::size_t length);
};
