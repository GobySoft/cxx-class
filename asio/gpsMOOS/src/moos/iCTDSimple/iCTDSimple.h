#ifndef iCTDSimple20170629H
#define iCTDSimple20170629H

#include "goby/util/linebasedcomms/serial_client.h"
#include "goby/moos/goby_moos_app.h"
#include "messages/ctd.pb.h"

#include "iCTDSimple_config.pb.h"

class CTDSimple : public GobyMOOSApp
{
  public:
    static CTDSimple* get_instance();
    static void delete_instance();
    
  private:
    CTDSimple(CTDSimpleConfig& cfg);
    ~CTDSimple();
    
    void loop();     // from GobyMOOSApp

    
    int parse_conductivity(const std::string& line, protobuf::CTDSample* ctd_ptr);
    int parse_double_field(const std::string& line, double* dbl_ptr);


    void handle_db_time(const CMOOSMsg& msg);
    
    
  private:
    static CTDSimple* inst_;    

    CTDSimpleConfig& cfg_;
    goby::util::SerialClient serial_;
};


#endif 
