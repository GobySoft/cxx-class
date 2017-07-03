#ifndef pGPSDisplayerH
#define pGPSDisplayerH

#include "messages/gps.pb.h"
#include "goby/moos/goby_moos_app.h"
#include "pGPSDisplayer_config.pb.h"

class GPSDisplayer : public GobyMOOSApp
{
  public:
    static GPSDisplayer* get_instance();
    static void delete_instance();
    
  private:
    GPSDisplayer(GPSDisplayerConfig& cfg);
    ~GPSDisplayer();
    
    void loop();     // from GobyMOOSApp

    void handle_gps_sample(const iGPSpb::GPSMessage& gps);
    
  private:
    GPSDisplayerConfig& cfg_;  
    static GPSDisplayer* inst_;    
};


#endif 
