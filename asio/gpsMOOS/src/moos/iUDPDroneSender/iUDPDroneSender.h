#include <boost/asio.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "iUDPDroneSender_config.pb.h"
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

class UDPDroneSender : public GobyMOOSApp
{
 public:
  static UDPDroneSender* get_instance();
  static void delete_instance();
 private:
  UDPDroneSender(multihop::UDPDroneSenderConfig& cfg);
  void loop();
  static UDPDroneSender* inst_;
  multihop::UDPDroneSenderConfig& cfg_;
  boost::asio::io_service io_service;
  udp::socket socket_;
  udp::endpoint receive_port_;
  udp::endpoint next_link_;
  udp::endpoint prev_link_;
  enum {max_length = 65536};
  multihop::UDPMessage msg;
  void do_send_forward(std::string tosend);
  void do_send_back(std::string tosend);
  void handle_udp_message(const multihop::UDPMessage& msg);
};
