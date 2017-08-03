#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "goby/util/binary.h"
#include "pLAMSSCatcher_config.pb.h"
#include "goby/moos/goby_moos_app.h"
#include "lamss/lib_lamss_protobuf/lamss_status.pb.h"
#include "lamss/lib_lamss_protobuf/lamss_deploy.pb.h"

using boost::asio::ip::udp;

class pLAMSSCatcher {
 public:
  static pLAMSSCatcher* get_instance();
  static void delete_instance();
 private:
  void loop();
  pLAMSSCatcher(multihop::pLAMSSCatcherConfig& cfg);
  void do_receive();
  pLAMSSCatcher* inst_;
  multihop::pLAMSSCatcherConfig cfg_;
  boost::asio::io_service io_service;
  udp::socket socket_;
  enum {max_length = 65536};
  char cstr_data_[max_length];
  std::string data_;
  
}
