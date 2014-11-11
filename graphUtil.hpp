#ifndef GRAPHUTIL_H_
#define GRAPHUTIL_H_

#ifndef GRAPHLIB_H_
    #include "graphLib.hpp"
#endif

#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <stack>
#include <iterator>


namespace GraphUtil {
//using namespace GraphLib;

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
        for( typename F::const_IdIter cIter = flowNetwork.beginNodeIter(curNodeId) ;          
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
        for( typename F::NeMapType::const_iterator mapIter = flowNetwork.beginNeMapIter();           
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
    typename F::const_IdIter  cIter = flowNetwork.beginNodeIter(source);
    for( ;cIter != flowNetwork.endNodeIter(source); ++cIter )
    {
        flowNetwork.getEdgeId(source, (*cIter));
        total += flowNetwork.getEdgeData(
                   flowNetwork.getEdgeId(source, (*cIter))
                   ).userData_.flow;
    }
    printf("Total Flow:%d\n", total);
}
struct Rec
{
    GraphLib::IdType nodeVId;   
    std::vector<GraphLib::IdType>::iterator iter;
};

//
// Auxiliary functions for getBackEdgeList inserting capability.
// Supports usages for std::insert(inserter iterator) and functor
//
template<typename Functor,typename DataType>
void insertHelper(Functor fun, const DataType& data)
{
    fun(data);
}

template<typename Container, typename DataType>
void insertHelper(std::back_insert_iterator<Container> iter, const DataType& data)
{
    *iter = data;
}
template<typename Container, typename DataType>
void insertHelper(std::insert_iterator<Container> iter, const DataType& data)
{
    *iter = data;
}


template<typename G, typename B>
void getBackEdgeList(G& graph, B& backEdgeListInserter)
{
    Rec curRec;
    int cnter = 0; //default starting number of node numbering system
    std::stack<Rec> st;
    std::map<GraphLib::IdType,bool> predecessorHT;
    std::vector<bool> checked(graph.nodeCount(), false);
    for( typename G::AdjListType::iterator iter = graph.beginNodeIter() ;
         iter != graph.endNodeIter();
         ++iter
       )
    {
        if( checked[cnter] )  continue;
        if( (*iter).empty() ) 
        {
            checked[cnter] = true;
            continue; //an isolated node
        }
        
        //Start to DFS starting from this node.
        predecessorHT.clear();
        curRec.nodeVId = cnter; 
        curRec.iter    = (*iter).begin();               
        do
        {   
            //check if it is back edge     
            if( curRec.iter != graph.endNodeIter(curRec.nodeVId))
            {   
                 if( checked[*curRec.iter] )
                 {                
                     if( (predecessorHT.end() != predecessorHT.find(*curRec.iter) && predecessorHT[*curRec.iter])                
                       )    
                     {
                        //backEdgeList.push_back( graph.neMap_[curRec.nodeVId][*curRec.iter] );
                        //backEdgeListInserter( graph.getEdgeId(curRec.nodeVId,*curRec.iter) );                 
                        insertHelper(backEdgeListInserter, graph.getEdgeId(curRec.nodeVId,*curRec.iter));

                     }
                     ++curRec.iter;
                 }
                 else
                 {                                  
                    //DFS                   
                    predecessorHT[curRec.nodeVId] = true;
                    checked[curRec.nodeVId]       = true;
                    st.push(curRec);
                    curRec.nodeVId = (*curRec.iter) ;
                    curRec.iter    = graph.beginNodeIter(curRec.nodeVId);
                 }
            }
            else if( !st.empty() )
            {
                //pop the last record
                predecessorHT[curRec.nodeVId] = false;
                curRec = st.top();
                st.pop();
                ++curRec.iter;
            }
            else
            {           
                //break if there is no record in the stack and all the out edges
                //are iterated
                break;
            }   
        
        } while(true);      
        
        ++cnter;
    }
    
    
    //print result
/*    for(typename B::iterator iter = backEdgeListInserter.getContainer().begin();
        iter != backEdgeListInserter.getContainer().end();
        ++iter
       )
    {
        printf("Directed Back Edge: (%d, %d)\n", graph.getEdgeData(*iter).fromId_.val(), graph.getEdgeData(*iter).toId_.val());
    }
*/
}

template<typename G>
void dagLogestPath(G& network, GraphLib::IdType source, std::vector<int>& dist, std::set<GraphLib::IdType>& skipEdge, std::map<GraphLib::IdType,GraphLib::IdType>& ans)
{
    //
    // Precondition:
    // network contains ONLY one source and one sink
    // it should be an acyclic network......
    //
    // NOTE: distance for every nodes is saved in one container.
    //       If the implementation take dist as an input parameter,
    //       then it is user's responsibility to initialize it correctly
    //       and maintain it when it is out of the scope of this function.
    //
    if( network.nodeCount() <= 0 ) return;
    
    //std::vector<GraphLib::IdType> mapping(network.adjList_.size(), network.adjList_.size());
    std::vector<int> inDegree(network.nodeCount());
    //initialize indegree;
    for(int idx = 0; idx < inDegree.size(); ++idx)
    {
        inDegree[idx] = network.getNodeData(idx).indegree_;
    }
    // Minus the inDegrees in the skip edges
    for(std::set<GraphLib::IdType>::iterator iter = skipEdge.begin();
        iter != skipEdge.end();
        ++iter
       )
    {
        GraphLib::IdType toId = network.getEdgeData(*iter).toId_;      
        --inDegree[toId];
    }
    
    //Please read the NOTE at the beginning of this function // it is changed to be an input parameter
    //std::vector<int> dist(network.adjList_.size(), INT_MIN);  
    //dist[source] = 0;
    
    // curNode --- parentNode in the path
    // it is changed to be an input parameter
    //std::map<GraphLib::IdType,GraphLib::IdType> ans; 
    std::queue<GraphLib::IdType> queue;     
    GraphLib::IdType curNodeId;
    queue.push(source); 
    while(!queue.empty())
    {
        curNodeId = queue.front();
        queue.pop();
        
        //printf("now checking:%d\n", curNodeId.val());
        for(typename G::ListType::const_iterator cIter = network.beginNodeIter(curNodeId);
            cIter != network.endNodeIter(curNodeId);
            ++cIter
           )
        {           
            //skip some unwanted edges, possibly backward edges
            if ( skipEdge.end() != skipEdge.find(network.getEdgeId(curNodeId,*cIter)) ) continue;
            
            //relaxation
            if( dist[*cIter] < 
                dist[curNodeId] + network.getEdgeData( network.getEdgeId(curNodeId,*cIter)).userData_.weight_ ) 
            {
                dist[*cIter] = dist[curNodeId] + network.getEdgeData( network.getEdgeId(curNodeId,*cIter)).userData_.weight_ ;         
                ans[*cIter] = curNodeId;
            }
            --inDegree[*cIter];
            if(inDegree[*cIter] <= 0)
            {               
                queue.push(*cIter);
            }
        }//for of checking outgoing edges
    }//while
    
#if 1   
{
    //detect cycle
    bool bCycleExist = false;
    GraphLib::IdType slow = curNodeId;
    GraphLib::IdType fast = ( ans.end() != ans.find(slow) ) ? ans[slow] : curNodeId;
    while( !(slow == fast) )
    {
        slow = ans[slow];
        
        if( !(ans.end() != ans.find(fast)) )
            break;
        fast = ans[fast];
         
        if( !(ans.end() != ans.find(fast)) )
            break;
        fast = ans[fast];           
    }
    //print answers
    //curNodeId = GraphLib::IdType(5);  
    //printf("Node: %d\n", curNodeId.val());
    if( !(slow == fast) )
    {
        while( ans.end() != ans.find(curNodeId))
        {       
            printf("Node: %d\n", curNodeId.val());
            curNodeId = ans[curNodeId];
        }
        printf("Node: %d\n", curNodeId.val());
    }
} // local scope, enforce destruction
#endif  
}

template<typename G>
bool longestPath(G& network, GraphLib::IdType source, std::map<GraphLib::IdType,GraphLib::IdType>& ans)
{
    if(network.nodeCount() <= 0 ) return false;
    
    //===========================================================
    //first separate the edges in network to backages and forward edges
    //===========================================================
    std::set<GraphLib::IdType> backEdgeSet;
    //Inserter<std::set<GraphLib::IdType> > inserter(backEdgeSet);
    std::insert_iterator<std::set<GraphLib::IdType> > 
    backInserter(backEdgeSet, backEdgeSet.begin());
    //getBackEdgeList(network, inserter);
    getBackEdgeList(network, backInserter);

    //================================================
    // start to find the longest path
    //================================================
    std::vector<int> dist(network.nodeCount(), INT_MIN); // intermediate dist
    dist[source] = 0;
    int count = 0;
    bool bRelaxed = false;
    do
    {
        bRelaxed = false;   
        dagLogestPath(network, source, dist, backEdgeSet, ans);
        
        for(std::set<GraphLib::IdType>::iterator iter = backEdgeSet.begin();
            iter != backEdgeSet.end();
            ++iter
           )
        {
            if(dist[network.getEdgeData(*iter).toId_ ]  < 
               dist[network.getEdgeData(*iter).fromId_] +
               network.getEdgeData(*iter).userData_.weight_
              )
            {
                dist[network.getEdgeData(*iter).toId_ ]  = 
                dist[network.getEdgeData(*iter).fromId_]  +
                network.getEdgeData(*iter).userData_.weight_;
                bRelaxed = true;
                ans[network.getEdgeData(*iter).toId_] = network.getEdgeData(*iter).fromId_;
            }           
        }
        ++count;
        if( count > backEdgeSet.size() && bRelaxed )
            break;
    } while(bRelaxed);
#if 0    
    GraphLib::IdType curNodeId = 5;
    while( ans.end() != ans.find(curNodeId))
    {       
        printf("Cyclic Node: %d\n", curNodeId.val());
        curNodeId = ans[curNodeId];
    }
    printf("Cyclic Node: %d\n", curNodeId.val());
#endif
for(std::map<GraphLib::IdType,GraphLib::IdType>::iterator iter=ans.begin();
    iter != ans.end();
    ++iter
   )
{
    printf("cyclic: %d %d\n", (*iter).first.val(), (*iter).second.val());
}   
    printf("Positive Cycle:%d\n",count > backEdgeSet.size() && bRelaxed);
    return (count > backEdgeSet.size() && bRelaxed);
}
} //name space

#endif
