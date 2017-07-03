#include "pCTDDisplayer.h"


using goby::glog;
using namespace goby::common::logger;
using goby::moos::operator<<;

boost::shared_ptr<CTDDisplayerConfig> master_config;
CTDDisplayer* CTDDisplayer::inst_ = 0;


 
int main(int argc, char* argv[])
{
    return goby::moos::run<CTDDisplayer>(argc, argv);
}


CTDDisplayer* CTDDisplayer::get_instance()
{
    if(!inst_)
    {
        master_config.reset(new CTDDisplayerConfig);
        inst_ = new CTDDisplayer(*master_config);
    }
    return inst_;
}

void CTDDisplayer::delete_instance()
{
    delete inst_;
}


CTDDisplayer::CTDDisplayer(CTDDisplayerConfig& cfg)
    : GobyMOOSApp(&cfg),
    cfg_(cfg)
{
    subscribe_pb("CTD_SAMPLE", &CTDDisplayer::handle_ctd_sample, this);
}


CTDDisplayer::~CTDDisplayer()
{
    
}

void CTDDisplayer::loop()
{
}

void CTDDisplayer::handle_ctd_sample(const protobuf::CTDSample& ctd)
{
    glog.is(VERBOSE) && glog << ctd.ShortDebugString() << std::endl;
}

