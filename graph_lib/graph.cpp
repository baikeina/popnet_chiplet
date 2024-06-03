#include"graph.h"

//#include <boost/graph/graph_utility.hpp>
//#include<boost/graph/adjacency_list.hpp>
//#include<boost/graph/breadth_first_search.hpp>
#include<boost/graph/graphviz.hpp>
#include<boost/graph/graphml.hpp>

#include<iostream>

#include<memory>

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"libboost_graph-vc142-mt-gd-x64-1_74.lib")
#pragma comment(lib,"libboost_regex-vc142-mt-gd-x64-1_74.lib")
#else
#pragma comment(lib,"libboost_graph-vc142-mt-x64-1_74.lib")
#pragma comment(lib,"libboost_regex-vc142-mt-x64-1_74.lib")
#endif
#endif

/* namespace boost
{
    BOOST_INSTALL_PROPERTY(vertex,bfso);
} */

using namespace std;
using namespace boost;

/* inline  */void getVertexName(const graph_t &g, vertexDes_t v, string &name)
{
    auto vnm = get(&vertex_info::name, g);
    name = vnm[v];
}
void myPrintGraphByVertexName(const graph_t &g)
{
    auto vnm = get(&vertex_info::name, g);
    graph_traits<graph_t>::edge_iterator ei, ei_end;
    cout << "edges: ";
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
        //cout << '(' << index[source(*ei, g)] << ", " << index[target(*ei, g)] << ") ";
        cout << '(' << vnm[source(*ei, g)] << ", " << vnm[target(*ei, g)] << ") ";
    }
    cout << endl;
}
/* inline  */void boostPrintGraphByVertexName(const graph_t &g)
{
    print_graph(g, get(&vertex_info::name, g));
}
/* inline  */void printGraph(const graph_t &g)
{
    //myPrintGraphByVertexName(g);
    boostPrintGraphByVertexName(g);
}
/* inline  */void removeVertexAndItsEdges(graph_t &g, vertexDes_t v)
{
    clear_vertex(v, g);
    remove_vertex(v, g);
}
void printGraphIndex(const graph_t &g)
{
    auto vertexList = vertices(g);
    cout << "Index:\n";
    for_each(vertexList.first, vertexList.second, [&](vertex_t v) {
        cout << "(" << g[v].name << ", " << g[v].index << ")\n";
    });
}
void printGraphColor(const graph_t &g)
{
    auto vertexList = vertices(g);
    cout << "Color:\n";
    for_each(vertexList.first, vertexList.second, [&](vertex_t v) {
        cout << "(" << g[v].name << ", " << g[v].color << ")\n";
    });
}
void resetIndex(graph_t& g)
{
	gsize_t index = 0;
	BGL_FORALL_VERTICES(v, g, graph_t) {
		put(&vertex_info::index, g, v, index++);
	}
}

gsize_t undirectedDegree(const graph_t&g,vertex_t v)
{
    return degree(v,g);
}
void resetColor(graph_t& g)
{
	BGL_FORALL_VERTICES(v, g, graph_t) {
		put(&vertex_info::color, g, v, boost::default_color_type());
	}
}

//设置属性
auto setDP(dynamic_properties&dp,graph_t&graph,ref_property_map<graph_t*,string>&graphName)
{
    //图属性
    dp.property("name",graphName);
    //节点属性
    dp.property("node_id",get(&vertex_info::name,graph));
    dp.property("energy_per_forwarding",get(&vertex_info::energyPerForwarding,graph));
    dp.property("in_traffic",get(&vertex_info::inTraffic,graph));
    dp.property("out_traffic",get(&vertex_info::outTraffic,graph));
    dp.property("pipeline_stage_delay",get(&vertex_info::pipelineStageDelay,graph));
    //边属性
    dp.property("id",get(edge_name,graph));
    //一跳的延迟
    dp.property("weight",get(edge_weight,graph));
    dp.property("energy_per_hop",get(edge_weight2,graph));
}

bool readGvGraph(const string&gvFile,graph_t&graph)
{
    //读取dot语言源代码文件
    dynamic_properties dp;
    //auto name=get(vertex_name,graph);
    /* auto name=get(&vertex_info::name,graph);
    dp.property("node_id",name);
    auto weight=get(edge_weight,graph);
    dp.property("weight",weight);
    ref_property_map<graph_t*,string>gname(get_property(graph,graph_name));
    dp.property("name",gname); */
    ref_property_map<graph_t*,string>gname(get_property(graph,graph_name));
    setDP(dp,graph,gname);
    ifstream ifs(gvFile);
    bool status=read_graphviz(ifs,graph,dp);
    ifs.close();
    resetIndex(graph);
    return status;
}
bool readGraphmlGraph(const string&graphmlFile,graph_t&graph)
{
    dynamic_properties dp;
    ref_property_map<graph_t*,string>gname(get_property(graph,graph_name));
    setDP(dp,graph,gname);
    ifstream ifs(graphmlFile);
    read_graphml(ifs,graph,dp);
    ifs.close();
    //myPrintGraphByVertexName(graph);
    resetIndex(graph);
    //printGraphIndex(graph);
    //print_graph(graph, get(&vertex_info::index, graph));
    return true;
}
bool readGraph(const string&graphFile,graph_t&graph)
{
    //return readGvGraph(graphFile,graph);
    return readGraphmlGraph(graphFile,graph);
}

bool writeGraphmlGraph(const string&graphmlFile,graph_t&graph)
{
    ofstream ofs(graphmlFile);
    dynamic_properties dp;
    ref_property_map<graph_t*,string>gname(get_property(graph,graph_name));
    setDP(dp,graph,gname);
    write_graphml(ofs,graph,get(&vertex_info::name,graph),dp);
    ofs.close();
    //resetIndex(graph);
    return true;
}
bool writeGraph(const string&graphmlFile,graph_t&graph)
{
    return writeGraphmlGraph(graphmlFile,graph);
}

bool writeGvGraph(const string&gvFile,graph_t&graph)
{
    ofstream ofs(gvFile);
    dynamic_properties dp;
    ref_property_map<graph_t*,string>gname(get_property(graph,graph_name));
    setDP(dp,graph,gname);
    write_graphviz_dp(ofs,graph,dp);
    return true;
}
