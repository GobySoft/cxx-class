// The basic server class that will be run at all points along the link.

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "messages/udp.pb.h"
#include "pLAMSSCatcher.h"
#include "goby/util/binary.h"
#include "pLAMSSCatcher_config.pb.h"
#include "goby/moos/goby_moos_app.h"
#include "lamss/lib_lamss_protobuf/lamss_status.pb.h"
#include "lamss/lib_lamss_protobuf/lamss_deploy.pb.h"

using boost::asio::ip::udp;

/**** Members necessary to be a MOOS app ****/
// setting up singleton for MOOSApp use
boost::shared_ptr<pLAMSSCatcherPB::pLAMSSCatcherConfig> master_config;
pLAMSSCatcher* pLAMSSCatcher::inst_ = 0;

pLAMSSCatcher* pLAMSSCatcher::get_instance()
{
  if (!inst_)
    {
      master_config.reset(new pLAMSSCatcherPB::pLAMSSCatcherConfig);
      inst_ = new pLAMSSCatcher(*master_config);
    }
  return inst_;
}

void pLAMSSCatcher::delete_instance()
{
  delete inst_;
}

// listens on port 9001 for LAMSS_DEPLOY messages
// if it finds any, publishes them to LAMSS_DEPLOY_MSG
void pLAMSSCatcher::loop()
{
  io_service.poll();

}
/**** end MOOSApp-specific member functions ****/

// Takes a config as a paramter, sets up a socket on port 9001, starts listening.
pLAMSSCatcher::pLAMSSCatcher(multihop::pLAMSSCatcherConfig& cfg)
  : GobyMOOSApp(&cfg),
    cfg_(cfg),
    socket_(io_service, udp::endpoint(udp::v4(),9001))
      
  {
    
    do_receive();
  }

// New data is immediately parsed to a message, which is then published.
void pLAMSSCatcher::do_receive()
{
  auto receive_handler = [this](boost::system::error_code ec, std::size_t bytes_recvd) 
    {
      if (!ec && bytes_recvd > 0 /*&& from previous link */)
	{
	  data_ = *(new std::string(cstr_data_, bytes_recvd));
	  msg.ParseFromString(data_);
	  publish_pb("LAMSS_DEPLOY_MSG", msg);
	  do_receive();
	}
      
      else
	{
	  do_receive();
	}
    };
  
  // the program listens on its designated port
  socket_.async_receive_from(boost::asio::buffer(cstr_data_, max_length), receive_port_, receive_handler);
  
}


int main(int argc, char* argv[])
{

  return goby::moos::run<pLAMSSCatcher>(argc,argv);

}
