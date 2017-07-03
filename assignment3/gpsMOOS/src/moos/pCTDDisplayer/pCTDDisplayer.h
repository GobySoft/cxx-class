#ifndef pCTDDisplayer20170629H
#define pCTDDisplayer20170629H

#include "messages/ctd.pb.h"

#include "goby/moos/goby_moos_app.h"

#include "pCTDDisplayer_config.pb.h"

class CTDDisplayer : public GobyMOOSApp
{
  public:
    static CTDDisplayer* get_instance();
    static void delete_instance();
    
  private:
    CTDDisplayer(CTDDisplayerConfig& cfg);
    ~CTDDisplayer();
    
    void loop();     // from GobyMOOSApp

    void handle_ctd_sample(const protobuf::CTDSample& ctd);
    
  private:
    CTDDisplayerConfig& cfg_;  
    static CTDDisplayer* inst_;    
};


#endif 
