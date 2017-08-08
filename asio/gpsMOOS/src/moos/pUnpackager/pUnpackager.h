#include "goby/moos/goby_moos_app.h"
#include "messages/udp.pb.h"
#include "pUnpackager_config.pb.h"

class Unpackager : public GobyMOOSApp
{
  public:
    static Unpackager* get_instance();
    static void delete_instance();
    
 private:
    Unpackager(multihop::UnpackagerConfig& cfg);
    ~Unpackager();
    void loop();     // from GobyMOOSApp
    void handle_udp_message(const multihop::UDPMessage& msg);
    
  private:
    multihop::UnpackagerConfig& cfg_;  
    static Unpackager* inst_;
};

