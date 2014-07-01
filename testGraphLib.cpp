#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

//main Graph structure
#include "GraphLib.hpp"

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
	IdType from;
	IdType to;
	int cap;
	int flow;
	char weight;
	
} handMade;

template<typename F>
int findAugmentPath(F& flowNetwork, IdType source, IdType sink, std::vector<IdType>& augmentPath )
{
	if (flowNetwork.nodeCount() <= 0 ) return 0;
	
	//=======================================
	// Start an Undirectional BFS search
	//=======================================
	std::set<IdType> checkHT;
	std::queue<IdType> pq;
	std::map<IdType, IdType> parentMap;
	
	IdType curNodeId;
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
			
			IdType edgeId = flowNetwork.getEdgeId( curNodeId, (*cIter) );
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
			  	IdType edgeId = flowNetwork.getEdgeId( (*mapIter).first, curNodeId );
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
		  curNodeId 	= parentMap[curNodeId];
		}
		augmentPath.push_back(curNodeId);				
		std::reverse(augmentPath.begin(), augmentPath.end());
		
		IdType edgeId;
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
void Edmond_Karp_MaxFlow(F& flowNetwork, IdType source, IdType sink)
{
	if (flowNetwork.nodeCount() <= 0 ) return;
	
	std::vector<IdType> augmentPath;
	int deltaFlow = findAugmentPath(flowNetwork, source, sink, augmentPath);	
		
	while (deltaFlow != 0 && augmentPath.front() == source && augmentPath.back() == sink )
	{
		//increase flow;
		IdType edgeId;
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
	
	IdType from(0);
	IdType to(1);
	tmpEdge.cap = 4;
	flowNetwork.addEdge(from, to, tmpEdge);
	
	from = 1 , to = 2;
	tmpEdge.cap = 6;
	flowNetwork.addEdge(from, to, tmpEdge);
	
	from = 2 , to = 3;
	tmpEdge.cap = 5;
	flowNetwork.addEdge(from, to, tmpEdge);
	
	from = 3 , to = 4;
	tmpEdge.cap = 4;
	flowNetwork.addEdge(from, to, tmpEdge);
//============================	
	from = 0 , to = 4;
	tmpEdge.cap = 3;
	flowNetwork.addEdge(from, to, tmpEdge);
	
	from = 4 , to = 5;
	tmpEdge.cap = 5;
	flowNetwork.addEdge(from, to, tmpEdge);
//==============================
    from = 0 , to = 6;
	tmpEdge.cap = 3;
	flowNetwork.addEdge(from, to, tmpEdge);	
	
	from = 6 , to = 7;
	tmpEdge.cap = 6;
	flowNetwork.addEdge(from, to, tmpEdge);	
	
	from = 7 , to = 8;
	tmpEdge.cap = 4;
	flowNetwork.addEdge(from, to, tmpEdge);	
	
	from = 7 , to = 4;
	tmpEdge.cap = 4;
	flowNetwork.addEdge(from, to, tmpEdge);	
	
	
	from = 8 , to = 9;
	tmpEdge.cap = 8;
	flowNetwork.addEdge(from, to, tmpEdge);	
	
	from =  9, to = 5;
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

    IdType from(0);
	IdType to(1);
	tmpEdge.cap = 15;
	flowNetwork.addEdge(from, to, tmpEdge);
	
	from =  0, to = 2;
	tmpEdge.cap = 10;
	flowNetwork.addEdge(from, to, tmpEdge);			
	
	from =  0, to = 3;
	tmpEdge.cap = 12;
	flowNetwork.addEdge(from, to, tmpEdge);	

//===================================================
	from =  1, to = 4;
	tmpEdge.cap = 5;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  1, to = 5;
	tmpEdge.cap = 5;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  1, to = 6;
	tmpEdge.cap = 5;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
//===============================================	
	from =  2, to = 4;
	tmpEdge.cap = 6;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  2, to = 5;
	tmpEdge.cap = 6;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  2, to = 6;
	tmpEdge.cap = 6;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
//================================================
	from =  3, to = 4;
	tmpEdge.cap = 12;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
//================================================
	from =  4, to = 7;
	tmpEdge.cap = 10;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  5, to = 7;
	tmpEdge.cap = 15;
	flowNetwork.addEdge(from, to, tmpEdge);					
	
	from =  6, to = 7;
	tmpEdge.cap = 15;
	flowNetwork.addEdge(from, to, tmpEdge);						
	
	
}
int main(int argc, char* argv[])
{	
    //eTuple a = {.cap=1,.flow=1,.weight=1} ;
	//GraphEdge<eTuple> edgeTest1(0,1,a);
	
	printf("IdType Size:%d\n",sizeof(IdType));
	printf("eTuple Size: %d\n", sizeof(eTuple));
	printf("Graph<eTuple> Size: %d\n", sizeof(GraphEdge<eTuple>));	
	printf("handMade Graph<eTuple> Size: %d\n", sizeof(handMade));			
    //GraphEdge<eTuple> edgeTest1(0,1,a)
    Graph<int, eTuple > graph;
    genTestCase< Graph<int,eTuple >, eTuple >(graph);
    Edmond_Karp_MaxFlow(graph, IdType(0), IdType(5) );
    
    Graph<int, eTuple > graph2;
    genTestCase2< Graph<int,eTuple >, eTuple >(graph2);
    Edmond_Karp_MaxFlow(graph2, IdType(0), IdType(7) );

	return EXIT_SUCCESS;
}
