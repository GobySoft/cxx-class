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
  void send_data(udp_proto::UDPMessage pb_msg);
  void address_udp(udp_proto::UDPMessage& outgoing);
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
  udp_proto::UDPMessage msg;
  bool isnew;
  short position; // Position of this server within the chain.
  void do_send_forward(std::string tosend);
  void do_send_back(std::string tosend);
};
