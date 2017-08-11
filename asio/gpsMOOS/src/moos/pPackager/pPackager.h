#include "goby/moos/goby_moos_app.h"
#include "pPackager_config.pb.h"
#include "messages/udp.pb.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

class Packager : public GobyMOOSApp
{
  public:
    static Packager* get_instance();
    static void delete_instance();
    
 private:
    Packager(multihop::PackagerConfig& cfg);
    ~Packager();
    void loop();     // from GobyMOOSApp
    void handle_pb_message(const CMOOSMsg& cmsg);
    multihop::PackagerConfig& cfg_;  
    static Packager* inst_;
    multihop::UDPMessage udp;
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

