#include <boost/asio.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "iUDPDroneReceiver_config.pb.h"
#include "goby/moos/goby_moos_app.h"

using boost::asio::ip::udp;

class UDPDroneReceiver : public GobyMOOSApp
{
 public:
  void do_receive();
  static UDPDroneReceiver* get_instance();
  static void delete_instance();
 private:
  UDPDroneReceiver(multihop::UDPDroneReceiverConfig& cfg);
  void loop();
  static UDPDroneReceiver* inst_;
  multihop::UDPDroneReceiverConfig& cfg_;
  boost::asio::io_service io_service;
  udp::socket socket_;
  udp::endpoint receive_port_;
  enum {max_length = 65536};
  char cstr_data_[max_length];
  std::string data_;
  multihop::UDPMessage msg;
  std::queue<multihop::UDPMessage> messages;
  std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
	result += buffer.data();
    }
    return result;
  };

};
