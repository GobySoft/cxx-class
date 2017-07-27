#ifndef pGPSDisplayerH
#define pGPSDisplayerH

#include "messages/gps.pb.h"
#include "goby/moos/goby_moos_app.h"
#include "lamss/lib_lamss_protobuf/lamss_status.pb.h"
#include "lamss/lib_lamss_protobuf/lamss_deploy.pb.h"
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
    void handle_liaison_commander(const CMOOSMsg& msg);
    void handle_lamss_status(const LAMSS_STATUS& lamss_status);
    void handle_lamss_deploy(const LAMSS_DEPLOY& lamss_deploy);
    
  private:
    gps_proto::PackagerConfig& cfg_;  
    static Packager* inst_;    
};


#endif 
