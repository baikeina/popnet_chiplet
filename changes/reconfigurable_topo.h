#ifndef RECONFIGURABLE_TOPO_H
#define RECONFIGURABLE_TOPO_H

#include"../sim_router.h"
//#include"../mess_queue.h"

#include<boost/multi_array.hpp>

#include<fstream>
#include<cstdint>

struct SFlow{
    int src,dst;
    double delay;
};
struct SReconfiguration{
    double reconfigPeriod;
    int64_t periodCnt,reconfigRouterCnt/* ,flowCnt */;
    std::vector<TAddressNumber>reconfigRouters;
    //boost::multi_array<SFlow,2>flows;
    std::vector<std::vector<SFlow>>flows;
};
class CReconfigTopoRouter:public CGraphTopo
{
protected:
    static SReconfiguration reconfigInfo;
    static graph_t curTopo;
    static std::int64_t curReconfigPeriodNumber;
    static time_type lastReconfigurationTime;
    static time_type nextReconfigurationTime;

    static TIntMatrix old_routingTable;
    static std::unique_ptr<std::vector<SLinkInfo>[]>old_portMap;
	static std::unique_ptr<std::unordered_map<TAddressNumber,TAddressNumber>[]>old_nextHop_port_map;

    static void readReconfigurationFile(const std::string&reconfigurationFilePath);
    static void setPortCnt_rtr(graph_t&topo);
    static void reconfigurateTopology(int64_t periodNumber);
    static void resetTopology();
    static void swapRoutingInfo();

    bool isEventAfterReconfiguration(time_type delay);
    bool isNearReconfiguration();
    virtual void routingAlg(const add_type&dst,const add_type&src,long s_ph,long s_vc);
    virtual time_type getWireDelay(long port);
    virtual void getNextAddress(add_type&nextAddress,long port);
    virtual long getWirePc(long port);
	virtual void getFromRouter(add_type&from,long port);
	virtual long getFromPort(long port);
public:
    static double getReconfigurationPeriod();
    static int64_t getCurrentReconfigurationPeriod();
    static void init_rtr();
    static void reconfigurate(time_type reconfigurationTime,time_type next_reconfigurationTime);

    CReconfigTopoRouter(long port_cnt,long vc_cnt,long in_buf_size,long out_buf_size,const add_type&address,long ary_size,long flit_size);

    friend class CRouter;
};

#endif
