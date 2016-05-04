#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

//main Graph structure
#include "graphLib.hpp"
#include "graphUtil.hpp"

#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <queue>

typedef struct _EdgeTuple_
{
    int cap;
    int flow;
    char weight;
} eTuple;

typedef struct _handMade_
{
    GraphLib::IdType from;
    GraphLib::IdType to;
    int cap;
    int flow;
    char weight;
    
} handMade;

/*
template<typename F>
int findAugmentPath(F& flowNetwork, GraphLib::IdType source, GraphLib::IdType sink, std::vector<GraphLib::IdType>& augmentPath )
{
    if (flowNetwork.nodeCount() <= 0 ) return 0;
    
    //=======================================
    // Start an Undirectional BFS search
    //=======================================
    std::set<GraphLib::IdType> checkHT;
    std::queue<GraphLib::IdType> pq;
    std::map<GraphLib::IdType, GraphLib::IdType> parentMap;
    
    GraphLib::IdType curNodeId;
    pq.push(source);
    checkHT.insert(source);
    while(!pq.empty())
    {
        curNodeId= pq.front();
        pq.pop();
        if(curNodeId == sink) break;
        printf("===========================\n");
        //Get Forward direction
        for( typename F::cIdIter cIter = flowNetwork.beginNodeIter(curNodeId) ;          
             cIter != flowNetwork.endNodeIter(curNodeId);
             ++cIter
           )
        {
            if( checkHT.end() != checkHT.find(*cIter) ) continue;
            
            GraphLib::IdType edgeId = flowNetwork.getEdgeId( curNodeId, (*cIter) );
            if( flowNetwork.getEdgeData(edgeId).userData_.cap <= 
                flowNetwork.getEdgeData(edgeId).userData_.flow ) continue;
            
            printf("touch:%d\n", (*cIter).val());                       
            pq.push(*cIter);
            parentMap[*cIter] = curNodeId;
            checkHT.insert(*cIter);         
        }
        
        //Get nodes in reverse direction
        for( typename F::neMapType::const_iterator mapIter = flowNetwork.beginNeMapIter();           
             mapIter != flowNetwork.endNeMapIter();
             ++mapIter
           )
        {
            if( (*mapIter).second.end() != (*mapIter).second.find( curNodeId ) &&
                !(checkHT.end() != checkHT.find( (*mapIter).first))
              ) 
              {
                GraphLib::IdType edgeId = flowNetwork.getEdgeId( (*mapIter).first, curNodeId );
                if( flowNetwork.getEdgeData(edgeId).userData_.flow > 0 ) 
                {
                    printf("touch:%d\n", (*mapIter).first.val());
                    pq.push((*mapIter).first);
                    parentMap[(*mapIter).first] = curNodeId;
                    checkHT.insert((*mapIter).first);
                }
              }
        }              
    }//while
    
    //Get augment path
    int deltaFlow = INT_MAX;
    if( curNodeId == sink ) //make sure there exists a path to sink
    {
        while( parentMap.end() != parentMap.find(curNodeId) )
        {
          augmentPath.push_back(curNodeId);
          curNodeId     = parentMap[curNodeId];
        }
        augmentPath.push_back(curNodeId);               
        std::reverse(augmentPath.begin(), augmentPath.end());
        
        GraphLib::IdType edgeId;
        for(int idx = 1; idx < augmentPath.size() ; ++idx)
        {
            //forward
            if( flowNetwork.hasEdge(augmentPath[idx-1], augmentPath[idx])  )
            {
                edgeId = flowNetwork.getEdgeId( augmentPath[idx-1], augmentPath[idx] );
                deltaFlow = std::min(deltaFlow, 
                                     flowNetwork.getEdgeData(edgeId).userData_.cap - 
                                     flowNetwork.getEdgeData(edgeId).userData_.flow
                                    );
            }
            else //backward
            {
                edgeId = flowNetwork.getEdgeId( augmentPath[idx], augmentPath[idx-1] );
                deltaFlow = std::min(deltaFlow, flowNetwork.getEdgeData(edgeId).userData_.flow);
            }   
        }
    }
    else
    {
        deltaFlow = 0;
    }
    
    return deltaFlow;
}
*/
/*
template <typename F>
void Edmond_Karp_MaxFlow(F& flowNetwork, GraphLib::IdType source, GraphLib::IdType sink)
{
    if (flowNetwork.nodeCount() <= 0 ) return;
    
    std::vector<GraphLib::IdType> augmentPath;
    int deltaFlow = findAugmentPath(flowNetwork, source, sink, augmentPath);    
        
    while (deltaFlow != 0 && augmentPath.front() == source && augmentPath.back() == sink )
    {
        //increase flow;
        GraphLib::IdType edgeId;
        for(int idx = 1; idx < augmentPath.size() ; ++idx)
        {
            //forward
            if( flowNetwork.hasEdge(augmentPath[idx-1], augmentPath[idx])  )
            {               
                edgeId = flowNetwork.getEdgeId(augmentPath[idx-1], augmentPath[idx]);
                flowNetwork.getEdgeData(edgeId).userData_.flow += deltaFlow;                
                printf("increase:%d %d %d\n",
                       flowNetwork.getEdgeData(edgeId).fromId_.val(),
                       flowNetwork.getEdgeData(edgeId).toId_.val(), 
                       flowNetwork.getEdgeData(edgeId).userData_.flow);
            }
            else //backward
            {
                edgeId = flowNetwork.getEdgeId(augmentPath[idx], augmentPath[idx-1]);               
                flowNetwork.getEdgeData(edgeId).userData_.flow -= deltaFlow; 
                printf("BBcrease:%d %d %d\n",
                       flowNetwork.getEdgeData(edgeId).fromId_.val(),
                       flowNetwork.getEdgeData(edgeId).toId_.val(), 
                       flowNetwork.getEdgeData(edgeId).userData_.flow);
            }   
        }
                
        // find again
        augmentPath.clear();
        deltaFlow = findAugmentPath(flowNetwork, source, sink, augmentPath);
    }
    
    //test
    int total = 0;
    typename F::cIdIter  cIter = flowNetwork.beginNodeIter(source);
    for( ;cIter != flowNetwork.endNodeIter(source); ++cIter )
    {
        flowNetwork.getEdgeId(source, (*cIter));
        total += flowNetwork.getEdgeData(
                   flowNetwork.getEdgeId(source, (*cIter))
                   ).userData_.flow;
    }
    printf("Total Flow:%d\n", total);
}
*/

template<typename F, typename T>
void genTestCase(F& flowNetwork)
{
    T tmpEdge = {0,0,0} ;
    
    flowNetwork.addNode(0);
    flowNetwork.addNode(1);
    flowNetwork.addNode(2);
    flowNetwork.addNode(3);
    flowNetwork.addNode(4);
    flowNetwork.addNode(5);
    flowNetwork.addNode(6);
    flowNetwork.addNode(7);
    flowNetwork.addNode(8);
    flowNetwork.addNode(9);
    flowNetwork.addNode(10);
    
    GraphLib::IdType from(0);
    GraphLib::IdType to(1);
    tmpEdge.cap = 4;
    flowNetwork.addEdge(from, to, tmpEdge);
    
    from = flowNetwork.getNodeIdByIdx(1) , to = flowNetwork.getNodeIdByIdx(2);
    tmpEdge.cap = 6;
    flowNetwork.addEdge(from, to, tmpEdge);
    
    from = flowNetwork.getNodeIdByIdx(2) , to = flowNetwork.getNodeIdByIdx(3);
    tmpEdge.cap = 5;
    flowNetwork.addEdge(from, to, tmpEdge);
    
    from = flowNetwork.getNodeIdByIdx(3) , to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 4;
    flowNetwork.addEdge(from, to, tmpEdge);
//============================  
    from = flowNetwork.getNodeIdByIdx(0) , to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 3;
    flowNetwork.addEdge(from, to, tmpEdge);
    
    from = flowNetwork.getNodeIdByIdx(4) , to = flowNetwork.getNodeIdByIdx(5);
    tmpEdge.cap = 5;
    flowNetwork.addEdge(from, to, tmpEdge);
//==============================
    from = flowNetwork.getNodeIdByIdx(0) , to = flowNetwork.getNodeIdByIdx(6);
    tmpEdge.cap = 3;
    flowNetwork.addEdge(from, to, tmpEdge); 
    
    from = flowNetwork.getNodeIdByIdx(6) , to = flowNetwork.getNodeIdByIdx(7);
    tmpEdge.cap = 6;
    flowNetwork.addEdge(from, to, tmpEdge); 
    
    from = flowNetwork.getNodeIdByIdx(7) , to = flowNetwork.getNodeIdByIdx(8);
    tmpEdge.cap = 4;
    flowNetwork.addEdge(from, to, tmpEdge); 
    
    from = flowNetwork.getNodeIdByIdx(7) , to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 4;
    flowNetwork.addEdge(from, to, tmpEdge); 
    
    
    from = flowNetwork.getNodeIdByIdx(8) , to = flowNetwork.getNodeIdByIdx(9);
    tmpEdge.cap = 8;
    flowNetwork.addEdge(from, to, tmpEdge); 
    
    from =  flowNetwork.getNodeIdByIdx(9), to = flowNetwork.getNodeIdByIdx(5);
    tmpEdge.cap = 7;
    flowNetwork.addEdge(from, to, tmpEdge);         
}

template<typename F, typename T>
void genTestCase2(F& flowNetwork)
{
    T tmpEdge = {0,0,0} ;   
    flowNetwork.addNode(0);
    flowNetwork.addNode(1);
    flowNetwork.addNode(2);
    flowNetwork.addNode(3);
    flowNetwork.addNode(4);
    flowNetwork.addNode(5);
    flowNetwork.addNode(6);
    flowNetwork.addNode(7);

    GraphLib::IdType from = flowNetwork.getNodeIdByIdx(0);
    GraphLib::IdType to = flowNetwork.getNodeIdByIdx(1);
    tmpEdge.cap = 15;
    flowNetwork.addEdge(from, to, tmpEdge);
    
    from = flowNetwork.getNodeIdByIdx(0), to = flowNetwork.getNodeIdByIdx(2);
    tmpEdge.cap = 10;
    flowNetwork.addEdge(from, to, tmpEdge);         
    
    from =  flowNetwork.getNodeIdByIdx(0), to = flowNetwork.getNodeIdByIdx(3);
    tmpEdge.cap = 12;
    flowNetwork.addEdge(from, to, tmpEdge); 

//===================================================
    from =  flowNetwork.getNodeIdByIdx(1), to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 5;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(1), to = flowNetwork.getNodeIdByIdx(5);
    tmpEdge.cap = 5;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(1), to = flowNetwork.getNodeIdByIdx(6);
    tmpEdge.cap = 5;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
//===============================================   
    from =  flowNetwork.getNodeIdByIdx(2), to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 6;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(2), to = flowNetwork.getNodeIdByIdx(5);
    tmpEdge.cap = 6;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(2), to = flowNetwork.getNodeIdByIdx(6);
    tmpEdge.cap = 6;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
//================================================
    from =  flowNetwork.getNodeIdByIdx(3), to = flowNetwork.getNodeIdByIdx(4);
    tmpEdge.cap = 12;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
//================================================
    from =  flowNetwork.getNodeIdByIdx(4), to = flowNetwork.getNodeIdByIdx(7);
    tmpEdge.cap = 10;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(5), to = flowNetwork.getNodeIdByIdx(7);
    tmpEdge.cap = 15;
    flowNetwork.addEdge(from, to, tmpEdge);                 
    
    from =  flowNetwork.getNodeIdByIdx(6), to = flowNetwork.getNodeIdByIdx(7);
    tmpEdge.cap = 15;
    flowNetwork.addEdge(from, to, tmpEdge);                     
    
    
}
int main(int argc, char* argv[])
{   
    //eTuple a = {.cap=1,.flow=1,.weight=1} ;
    //GraphLib::GraphEdge<eTuple> edgeTest1(0,1,a);
    
    printf("IdType Size:%d\n",sizeof(GraphLib::IdType));
    printf("eTuple Size: %d\n", sizeof(eTuple));
    printf("GraphLib::Graph<eTuple> Size: %d\n", sizeof(GraphLib::GraphEdge<eTuple>));  
    printf("handMade GraphLib::Graph<eTuple> Size: %d\n", sizeof(handMade));          
    //GraphLib::GraphEdge<eTuple> edgeTest1(0,1,a)
    GraphLib::Graph<int, eTuple > graph;
    genTestCase< GraphLib::Graph<int,eTuple >, eTuple >(graph);
    GraphUtil::Edmond_Karp_MaxFlow(graph, graph.getNodeIdByIdx(0), graph.getNodeIdByIdx(5) );
    
    GraphLib::Graph<int, eTuple > graph2;
    genTestCase2< GraphLib::Graph<int,eTuple >, eTuple >(graph2);
    GraphUtil::Edmond_Karp_MaxFlow(graph2, graph2.getNodeIdByIdx(0), graph2.getNodeIdByIdx(7) );

    return EXIT_SUCCESS;
}
