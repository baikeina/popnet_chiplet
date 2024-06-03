//参考
//https://www.boost.org/doc/libs/1_79_0/libs/graph/doc/adjacency_list.html
//https://www.cnblogs.com/shanzhizi/articles/2609934.html
//https://blog.sciencenet.cn/home.php?mod=space&uid=562235&do=blog&quickforward=1&id=543168

#include"reconfigurable_topo.h"

#include"../mess_queue.h"

using namespace std;
using namespace boost;

#ifdef FILTERING
constexpr int64_t RECONFIGURATION_PERIOD_NUMBER_0=START_RECONFIGURATION_PERIOD_NUMBER;
#else
constexpr int64_t RECONFIGURATION_PERIOD_NUMBER_0=0;
#endif

//静态变量定义！！！
SReconfiguration CReconfigTopoRouter::reconfigInfo;
graph_t CReconfigTopoRouter::curTopo;
int64_t CReconfigTopoRouter::curReconfigPeriodNumber;
time_type CReconfigTopoRouter::lastReconfigurationTime;
time_type CReconfigTopoRouter::nextReconfigurationTime;

TIntMatrix CReconfigTopoRouter::old_routingTable;
unique_ptr<vector<CReconfigTopoRouter::SLinkInfo>[]>CReconfigTopoRouter::old_portMap;
unique_ptr<std::unordered_map<TAddressNumber,TAddressNumber>[]>CReconfigTopoRouter::old_nextHop_port_map;

void readReconfigInfo(const string&reconfigFilePath,SReconfiguration&reconfigInfo)
{
    ifstream ifs(reconfigFilePath);
    ifs>>reconfigInfo.periodCnt>>reconfigInfo.reconfigPeriod>>reconfigInfo.reconfigRouterCnt/* >>reconfigInfo.flowCnt */;
    int64_t i,j;
    int64_t t;
    reconfigInfo.reconfigRouters.reserve(reconfigInfo.reconfigRouterCnt);
    for(i=0;i<reconfigInfo.reconfigRouterCnt;i++){
        ifs>>t;
        reconfigInfo.reconfigRouters.push_back(t);
    }
    /* reconfigInfo.flows.resize(extents[reconfigInfo.periodCnt][reconfigInfo.flowCnt]);
    for(i=0;i<reconfigInfo.periodCnt;i++){
        for(j=0;j<reconfigInfo.flowCnt;j++){
            ifs>>reconfigInfo.flows[i][j].src>>reconfigInfo.flows[i][j].dst;
        }
    } */
    reconfigInfo.flows.reserve(reconfigInfo.periodCnt);
    SFlow ft;
    for(i=0;i<reconfigInfo.periodCnt;i++){
        reconfigInfo.flows.emplace_back();
        ifs>>t;
        reconfigInfo.flows.back().reserve(t);
        for(j=0;j<t;j++){
            ifs>>ft.src>>ft.dst>>ft.delay;
            reconfigInfo.flows.back().push_back(ft);
        }
    }
}
void CReconfigTopoRouter::readReconfigurationFile(const string&reconfigurationFilePath)
{
    readReconfigInfo(reconfigurationFilePath,reconfigInfo);
}

void CReconfigTopoRouter::setPortCnt_rtr(graph_t&topo)
{
    std::unordered_set<TAddressNumber>interposerRouterSet(reconfigInfo.reconfigRouters.begin(),reconfigInfo.reconfigRouters.end());
    vPortCount.resize(vertexCnt);
	auto nodes=vertices(topo);
	auto name=get(&vertex_p::name,topo);
	for_each(nodes.first,nodes.second,[&](const vertex_t&v){
		TAddressNumber add=stol(name[v]);
        //注意：中介层路由器没有本地端口
        //方便起见，加上本地端口
        //最多4条可重构链路
		vPortCount[add]=degree(v,topo)+1+(interposerRouterSet.count(add)>0?4:0);
	});
}

void CReconfigTopoRouter::reconfigurateTopology(int64_t periodNumber)
{
    resetTopology();
    for(auto&newLink:reconfigInfo.flows[periodNumber]){
        /* auto r=add_edge(topoVertices[newLink.src],topoVertices[newLink.dst],curTopo);
        auto delay=get(edge_weight,curTopo);
        if(r.second)delay[r.first]=newLink.delay; */
        edge_p ep{newLink.delay};
        add_edge(topoVertices[newLink.src],topoVertices[newLink.dst],ep,curTopo);
    }
}


void CReconfigTopoRouter::resetTopology()
{
    curTopo=topo0;
    setTopoVertices(curTopo);
}
void CReconfigTopoRouter::swapRoutingInfo()
{
    swap(old_routingTable,routingTable);
    swap(old_portMap,portMap);
    swap(old_nextHop_port_map,nextHop_port_map);
}

double CReconfigTopoRouter::getReconfigurationPeriod()
{
    return reconfigInfo.reconfigPeriod;
}

int64_t CReconfigTopoRouter::getCurrentReconfigurationPeriod()
{
    return curReconfigPeriodNumber;
}

void CReconfigTopoRouter::init_rtr()
{
    readTopo();
    otherInit();
    readReconfigurationFile(configuration::ap().getReconfigurationFilePath());
    curReconfigPeriodNumber=RECONFIGURATION_PERIOD_NUMBER_0;
    lastReconfigurationTime=TIME_0;
    old_routingTable.resize(extents[vertexCnt][vertexCnt]);
    setPipelineDelay(topo0);
    setPortCnt_rtr(topo0);
    resetTopology();
    calRoutingTable(curTopo);
    swapRoutingInfo();
}

void CReconfigTopoRouter::reconfigurate(time_type reconfigurationTime,time_type next_reconfigurationTime)
{
    if(curReconfigPeriodNumber<reconfigInfo.periodCnt){
        reconfigurateTopology(curReconfigPeriodNumber);
        //swapRoutingInfo();
        calRoutingTable(curTopo);
        lastReconfigurationTime=reconfigurationTime;
        nextReconfigurationTime=next_reconfigurationTime;
        curReconfigPeriodNumber++;
    }
}

CReconfigTopoRouter::CReconfigTopoRouter(long port_cnt,long vc_cnt,long in_buf_size,long out_buf_size,const add_type&address,long ary_size,long flit_size)
    :CGraphTopo(port_cnt,vc_cnt,in_buf_size,out_buf_size,address,ary_size,flit_size)
{
    //nothing
}

bool CReconfigTopoRouter::isEventAfterReconfiguration(time_type delay)
{
    return mess_queue::m_pointer().current_time()-delay>=lastReconfigurationTime;
    //return true;
}
bool CReconfigTopoRouter::isNearReconfiguration()
{
    constexpr time_type RECONFIGURATION_INTERVAL=20;
    return mess_queue::m_pointer().current_time()+RECONFIGURATION_INTERVAL>=nextReconfigurationTime;
}
void CReconfigTopoRouter::routingAlg(const add_type&dst,const add_type&src,long s_ph,long s_vc)
{
    //tableRoutingAlg(dst,src,s_ph,s_vc,routingTable,nextHop_port_map);
    if(isNearReconfiguration()){
        tableRoutingAlg(dst,src,s_ph,s_vc,old_routingTable,old_nextHop_port_map);
    }else{
        tableRoutingAlg(dst,src,s_ph,s_vc,routingTable,nextHop_port_map);
    }
}
time_type CReconfigTopoRouter::getWireDelay(long port)
{
    return tableWireDelay(port,portMap);
}
void CReconfigTopoRouter::getNextAddress(add_type&nextAddress,long port)
{
    getNeighbour(nextAddress,port,portMap);
}
long CReconfigTopoRouter::getWirePc(long port)
{
    return getNeighbourPort(port,portMap);
}
void CReconfigTopoRouter::getFromRouter(add_type&from,long port)
{
    //getNeighbour(from,port,isEventAfterReconfiguration(CREDIT_DELAY_)?portMap:old_portMap);
    getNeighbour(from,port,portMap);
}
long CReconfigTopoRouter::getFromPort(long port)
{
    //return getNeighbourPort(port,isEventAfterReconfiguration(CREDIT_DELAY_)?portMap:old_portMap);
    return getNeighbourPort(port,portMap);
}
