#include"shortest_path_routing.h"

#include"../index.h"

using namespace std;
using namespace boost;

constexpr double PIPELINE_STAGE_DELAY_0=PIPE_DELAY_;
constexpr int PIPELINE_STAGE_NUMBER=5;
constexpr double ROUTING_DELAY_0=PIPELINE_STAGE_DELAY_0*PIPELINE_STAGE_NUMBER;
constexpr double LINK_DELAY_0=WIRE_DELAY_;

//TODO: 及时修改
//done
template<typename PM,typename V>
inline double routing_delay(PM&propertyMap,V&v)
{
    //return ROUTING_DELAY_0;
    return propertyMap[v]*PIPELINE_STAGE_NUMBER;
}

template<typename PM,typename E>
inline double link_delay(PM&propertyMap,E&e)
{
    return propertyMap[e];
    //return LINK_DELAY_0;
}

void resetIntMatrix(TIntMatrix&table,size_t n,int value)
{
    table.resize(extents[n][n]);
    for(size_t i=0;i<n;++i)fill(table[i].begin(),table[i].end(),value);
}
inline void setDoubleHop(TNextHopTable&table,size_t x,size_t y,int x_next,int y_next)
{
    table[x][y]=x_next;
    table[y][x]=y_next;
}
void resetMatrix(TMatrix&matrix,size_t n,double value)
{
    matrix.resize(extents[n][n]);
    //for(size_t i=0;i<n;++i)fill(matrix[i].begin(),matrix[i].end(),value);
    for(size_t i=0;i<n;++i){
        for(size_t j=0;j<n;++j)matrix[i][j]=(i==j)?0:value;
    }
}
inline void setDoubleValue(TMatrix&delayTable,size_t x,size_t y,double delay)
{
    delayTable[x][y]=delay;
    delayTable[y][x]=delay;
}
void calculateShortestPathTables(graph_t&g,TMatrix&delayTable,TMatrix&energyTable,TNextHopTable&nextHop)
{
    const double INF_WEIGHT=numeric_limits<double>::infinity();
    size_t n=num_vertices(g),i,j,k;
    resetIntMatrix(nextHop,n,-1);
    for(i=0;i<n;i++)nextHop[i][i]=i;
    resetMatrix(delayTable,n,INF_WEIGHT);
    resetMatrix(energyTable,n,INF_WEIGHT);
    //resetIndex(g);
    auto index=get(&vertex_info::index,g);
    auto vertexList=vertices(g);
    auto forwardEnergyMap=get(&vertex_info::energyPerForwarding,g);
    auto pipelineStageDelay=get(&vertex_info::pipelineStageDelay,g);
    vector<double>forwardEnergy(n);
    vector<double>routerDelay(n);
    for_each(vertexList.first,vertexList.second,[&](const vertex_t&v){
        size_t idx=index[v];
        forwardEnergy[idx]=forwardEnergyMap[v];
        routerDelay[idx]=routing_delay(pipelineStageDelay,v);
    });
    auto edgeList=edges(g);
    auto delay=get(edge_weight,g);
    auto hopEnergyMap=get(edge_weight2,g);
    for_each(edgeList.first,edgeList.second,[&](const edge_t&e){
        //setDoubleValue(delayTable,index[source(e,g)],index[target(e,g)],delay[e]);
        int x=index[source(e,g)],y=index[target(e,g)];
        //假设路由器的转发需要时间（包括发送方和接收方的路由器）
        setDoubleValue(delayTable,x,y,link_delay(delay,e)+routerDelay[x]+routerDelay[y]);
        setDoubleHop(nextHop,x,y,y,x);
        setDoubleValue(energyTable,x,y,forwardEnergy[x]+hopEnergyMap[e]+forwardEnergy[y]);
    });
    //Floyd-Warshall
    for(k=0;k<n;++k){
        for(i=0;i<n;++i){
            for(j=0;j<n;++j){
                //if(i==j||j==k||i==k)continue;
                double t;
                t=delayTable[i][k]+delayTable[k][j];
                //路由器k的转发延迟算了2次，所以要减去1次
                if(i!=j&&j!=k&&i!=k)t-=routerDelay[k];
                if(t<delayTable[i][j]){
                    delayTable[i][j]=t;
                    //需要减去路由器k的转发能耗
                    energyTable[i][j]=energyTable[i][k]+energyTable[k][j]-forwardEnergy[k];
                    setDoubleHop(nextHop,i,j,nextHop[i][k],nextHop[j][k]);
                }
            }
        }
    }
}
