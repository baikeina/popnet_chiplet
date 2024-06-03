#ifndef SHORTEST_PATH_ROUTING_H
#define SHORTEST_PATH_ROUTING_H

#include"graph.h"

#include<boost/multi_array.hpp>

#include<cmath>

typedef boost::multi_array<double,2> TMatrix;
typedef TMatrix TDelayMatrix;

//设当前位置为x，目的为y，则下一跳为table[x][y]
typedef boost::multi_array<int,2> TIntMatrix;
typedef TIntMatrix TNextHopTable;

void resetIntMatrix(TIntMatrix&table,size_t n,int value);
void resetMatrix(TMatrix&matrix,size_t n,double value);

void calculateShortestPathTables(graph_t&g,TMatrix&delayTable,TMatrix&energyTable,TNextHopTable&nextHop);

#endif
