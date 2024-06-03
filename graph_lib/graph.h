//图相关的定义

//参考
//https://blog.csdn.net/u011630575/article/details/80837629
//https://blog.csdn.net/dc2010_/article/details/39758641

#ifndef GRAPH_H
#define GRAPH_H

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/breadth_first_search.hpp>
#include<boost/config.hpp>
#include <boost/graph/graph_utility.hpp>

#include<vector>
#include<unordered_set>
#include<set>
#include<string>
#include<list>
#include <forward_list>

struct vertex_info{
    std::size_t bfso;
    boost::default_color_type color;
    //std::size_t discover_time;
    std::size_t index;
    std::string name;
    double energyPerForwarding;
    int inTraffic,outTraffic;
    double pipelineStageDelay;

	vertex_info()
		:bfso(0),color(),index(0),name(),energyPerForwarding(0),inTraffic(0),outTraffic(0),pipelineStageDelay(0){}
};

struct SEdgeInfo{
    std::string name;
    double delay,energyPerHop;
};

/* enum vertex_bfso_t{
    vertex_bfso
};
namespace boost
{
    BOOST_INSTALL_PROPERTY(vertex,bfso);
}
 */
typedef boost::undirectedS edge_type;
/* typedef boost::property<boost::vertex_name_t,std::string,
    boost::property<vertex_bfso_t,std::size_t,
    vertex_info>> vertex_p; */
//typedef boost::property<boost::vertex_name_t,std::string> vertex_p;
typedef vertex_info vertex_p;
typedef boost::property<boost::edge_weight_t,double,//delay
    boost::property<boost::edge_name_t,std::string,
    boost::property<boost::edge_weight2_t,double//energy_per_forwarding
>>> edge_p;
typedef boost::property<boost::graph_name_t,std::string> graph_p;
//typedef boost::vecS vertexContainer_t;
typedef boost::listS vertexContainer_t;
//typedef boost::vecS edgeContainer_t;
typedef boost::listS edgeContainer_t;
typedef boost::adjacency_list<edgeContainer_t,vertexContainer_t,edge_type,vertex_p,edge_p,graph_p> graph_t;
//typedef adjacency_list<vecS,vecS,undirectedS,vertex_info> graph_t;
typedef boost::graph_traits<graph_t>::vertices_size_type gsize_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor vertexDes_t;
typedef boost::graph_traits<graph_t>::edge_descriptor edgeDes_t;

typedef std::set<std::string> vertexList_t;
typedef std::set<vertexList_t> vertexCutsetList_t;
//typedef std::forward_list<edgeDes_t> edgeList_t;
typedef std::list<edgeDes_t> edgeList_t;
typedef std::forward_list<edgeList_t> edgeCutsetList_t;

typedef vertexDes_t vertex_t;
typedef edgeDes_t edge_t;

void getVertexName(const graph_t &g, vertexDes_t v, std::string &name);
void myPrintGraphByVertexName(const graph_t &g);
void boostPrintGraphByVertexName(const graph_t &g);
void printGraph(const graph_t &g);
void removeVertexAndItsEdges(graph_t &g, vertexDes_t v);
void printGraphIndex(const graph_t &g);
void printGraphColor(const graph_t &g);
void resetIndex(graph_t& g);

gsize_t undirectedDegree(const graph_t&g,vertex_t v);
void resetColor(graph_t& g);

bool readGvGraph(const std::string&gvFile,graph_t&graph);
bool readGraphmlGraph(const std::string&graphmlFile,graph_t&graph);
bool readGraph(const std::string&graphFile,graph_t&graph);

bool writeGraphmlGraph(const std::string&graphmlFile,graph_t&graph);
bool writeGraph(const std::string&graphmlFile,graph_t&graph);
bool writeGvGraph(const std::string&gvFile,graph_t&graph);

#endif
