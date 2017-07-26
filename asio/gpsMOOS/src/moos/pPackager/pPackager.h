#ifndef pGPSDisplayerH
#define pGPSDisplayerH

#include "messages/gps.pb.h"
#include "goby/moos/goby_moos_app.h"
#include "pPackager_config.pb.h"

class Packager : public GobyMOOSApp
{
  public:
    static Packager* get_instance();
    static void delete_instance();
    
 private:
    Packager(gps_proto::PackagerConfig& cfg);
    ~Packager();
    
    void loop();     // from GobyMOOSApp

    void handle_gps_message(const gps_proto::GPSMessage& gps);
    
  private:
    gps_proto::PackagerConfig& cfg_;  
    static Packager* inst_;    
};


#endif 
