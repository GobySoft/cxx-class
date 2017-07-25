#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "udp.pb.h"

using boost::asio::ip::udp;

class server{
 public:
  server(boost::asio::io_service& io_service, short pos, short port, boost::asio::ip::address nextIP, short nextport, boost::asio::ip::address prevIP, short prevport);
  void do_receive();
  void send_data(std::string msg);
  bool hasnew();
  std::string get_str();
 private:
  udp::socket socket_;
  udp::endpoint receive_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum {max_length = 65536};
  char cstr_data_[max_length];
  std::string data_;
  bool isnew;
  short position; // Position of this server within the chain.
  void do_send_forward(std::size_t length);
  void do_send_back(std::size_t length);
};
