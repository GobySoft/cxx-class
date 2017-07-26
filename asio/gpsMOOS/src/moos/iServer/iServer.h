#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "messages/gps.pb.h"
#include "goby/util/binary.h"
#include "iServer_config.pb.h"
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

class iServer : public GobyMOOSApp
{
 public:
  void do_receive();
  bool hasnew();
  static iServer* get_instance();
  static void delete_instance();
  std::string get_str();
 private:
  iServer(iServerPB::iServerConfig& cfg);
  void loop();
  static iServer* inst_;
  iServerPB::iServerConfig& cfg_;
  boost::asio::io_service io_service;
  udp::socket socket_;
  udp::endpoint receive_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum {max_length = 65536};
  char cstr_data_[max_length];
  std::string data_;
  udp_proto::UDPMessage msg;
  bool isnew;
  short position; // Position of this iServer within the chain.
  void do_send_forward(std::string tosend);
  void do_send_back(std::string tosend);
  void handle_udp_message(const udp_proto::UDPMessage& msg);
};
