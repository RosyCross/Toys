#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <queue>
#include <stack>

#include "graphUtil.hpp"
#include "inserter.hpp"

// Problems Found:
// 1. This implementation cannot delete nodes/edges dynamically because
//    the underlying data structure used is std::vector
//
// 2. THIS LIBRARY DOES NOT CHECK PRECONDITIONS of WORKING FUNCTIONS. USER SHOULD
//    MAKE SURE THE PRECONDITONS ARE SATISFIED BEFORE CALLING WORKING FUNCTIONS.
//    BEWARE: THE WORKING FUNCTIONS IN THIS FILE IS FOR FLOW NETWORK WHICH MEANS
//    THERE IS ONE SOURCE and ONE SINK
//
// 3. Should add more getter to encapsulate internal data structures of Graph and others
//
// 4. Because operator[] of std::map does not have const version, it prohibits to add
//    const qualifier for class template Graph
//

//
// class _IdType is an experimental class try to solve the 
// overloading problem of the member function of template class.
// This class is kind of use the mechanism of choosing
// overloading member function by using different input parameter class types.
// PRICE: so many temporary objects and conversions.
//
/*
class _final 
{
	private:
		friend class _IdType;
		_final()  {};
		virtual ~_final() {};
		
};
class _IdType : public virtual _final
{
	public:
		explicit _IdType(): idVal_(0){}
		_IdType(int value): idVal_(value){}
		operator int()    const { return idVal_;};
		operator size_t() const { return (size_t)idVal_;};
		bool operator<(const _IdType& rhs)  const {return idVal_  < rhs.idVal_;};
		bool operator>(const _IdType& rhs)  const {return idVal_  > rhs.idVal_;};
		bool operator==(const _IdType& rhs) const {return idVal_ == rhs.idVal_;};
		bool operator>=(const _IdType& rhs) const {return idVal_ >= rhs.idVal_;};
		bool operator<=(const _IdType& rhs) const {return idVal_ <= rhs.idVal_;};
		
		bool operator<(int rhs) const {return idVal_  < rhs;};
		bool operator>(int rhs) const {return idVal_  > rhs;};
		bool operator==(int rhs) const {return idVal_ == rhs;};		
		bool operator>=(int rhs) const {return idVal_ >= rhs;};
		
		//may not be necessary
		const _IdType& operator=(int newVal )  { idVal_ = newVal; return idVal_; };
		int val() const {return idVal_;}
		
	private:
		int idVal_;
};
*/
/*
//class my : public _IdType {};
typedef _IdType IdType;

//typedef int IdType;
template<typename T>
class GraphNode
{
	public:
	GraphNode(const T& userData): userData_(userData), indegree_(0), Id_(INT_MIN){};
	
		
	T userData_;
	int indegree_;
    IdType Id_;	
};

class GraphEdge
{
	public:
	GraphEdge(IdType from,IdType to):fromId_(from),toId_(to), weight_(0) {};
		
	IdType fromId_;
	IdType toId_;
	int weight_;
};

template<typename T>
class Graph
{
	public: // auxiliary typedefs for convenience
		typedef std::vector< std::vector<IdType> > adjListType;
		typedef std::vector< IdType >              listType; 
        
		typedef std::vector< GraphNode<T> >        nodeDepotType;
        typedef std::vector<GraphEdge>             edgeDepotType;
		
		typedef std::vector<IdType>::iterator       idIter;	
		typedef std::vector<IdType>::const_iterator cIdIter;
		 
    public:
    	IdType addNode(const T& userData);
    	IdType addEdge(IdType from, IdType to,int weight);
    	IdType addEdge(const T& userDataFrom, const T& userDataTo, int weight);
    	IdType addEdge(IdType from, const T& userDataTo, int weight);
    	//use Hash will be good. But STL::MAP is not a real HASH
	    std::vector< std::vector<IdType> > adjList_;
	    std::map<IdType, std::map<IdType, IdType> > neMap_; //cannot think of a concise model for this
	    //depots
		std::vector< GraphNode<T> > nodeDepot_;
		std::vector<GraphEdge> edgeDepot_;	
	
	private:
	    IdType valueToId(size_t value) {return (IdType)value;}	    	
};


template<typename T>
IdType Graph<T>::addNode(const T& userData)
{
	nodeDepot_.push_back(GraphNode<T>(userData));
	nodeDepot_.back().Id_ = nodeDepot_.size()-1;
	//remove this line if adjList becomes hash
	adjList_.push_back(std::vector<IdType>());//a place holder		
	return(IdType(nodeDepot_.size()-1));
}

template<typename T>
IdType Graph<T>::addEdge(IdType from, IdType to, int weight)
{	
	assert( from < (int)nodeDepot_.size() && 
	        from >= 0 
	      );
	assert( to < (int)nodeDepot_.size() && 
	        to >= 0 
	      );      
	if( !(from < (int)nodeDepot_.size() && from >= 0 ) )      
	    return INT_MIN;
	    
	if( !(to < (int)nodeDepot_.size() && to >= 0 ) )    
	    return INT_MIN;
	    
	edgeDepot_.push_back(GraphEdge(from,to));
	edgeDepot_.back().fromId_ = from;
	edgeDepot_.back().toId_   = to;
	edgeDepot_.back().weight_ = weight;
    ++nodeDepot_[to].indegree_;
	//adjList[from][to] = edgeDepot_.size()-1;
	adjList_[from].push_back(to);
	neMap_[from][to] = edgeDepot_.size()-1;
	//adjList[from].push_back(edgeDepot_.size()-1);
	printf("F-T:(%d,%d) size:%d\n",(int)from , (int)to, adjList_[from].size());
	return(IdType(edgeDepot_.size()-1));
}

template<typename T>
IdType Graph<T>::addEdge(const T& userDataFrom, const T& userDataTo, int weight)
{
	IdType fromId = addNode(userDataFrom);
	IdType toId   = addNode(userDataTo);
	return addEdge(fromId, toId, weight);
}

template<typename T>
IdType Graph<T>::addEdge(IdType from, const T& userDataTo, int weight)
{
	assert( from < (int)nodeDepot_.size() && 
	        from >= 0 
	      );

	if( !(from < (int)nodeDepot_.size() && from >= 0 ) )      
	    return INT_MIN;

    IdType to = addNode(userDataTo);
	return addEdge(from,to, weight);	      
}
*/
//
// An auxiliary inster
//
#if 1
#include "inserter.hpp"
template<>
void Inserter<std::vector<GraphLib::IdType> >::operator()(const GraphLib::IdType& input) 
{
	container_.push_back(input);
};
template<>
void Inserter<std::set<GraphLib::IdType> >::operator()(const GraphLib::IdType& input) 
{
	container_.insert(input);
};
#endif

/*
//======================================================================//
// Operations that can act on Network Flow
// 
template<typename G>
void adjListPrint(G& graph)
{
    for(int idx = 0; idx < graph.adjList_.size(); ++idx)
    {    	
    	for(typename G::cIdIter iter = graph.adjList_[idx].begin(); 
		    iter != graph.adjList_[idx].end(); 
			++iter)
    	{
    		printf("From-To: (%d,%d) Weight: %d\n", idx, (*iter).val(),
			        graph.edgeDepot_[graph.neMap_[idx][(*iter)]].weight_);
    	}
    }
    for(int idx = 0; idx < graph.edgeDepot_.size(); ++idx)
    {
    	printf("(%d,%d,%d)\n",
		       (int)graph.edgeDepot_[idx].fromId_,
			   (int)graph.edgeDepot_[idx].toId_,
			   (int)graph.edgeDepot_[idx].weight_);
    }
}

template<typename G>
void DFS(G& graph, IdType& nodeId, std::vector<bool>& check)
{
	printf("Current Node:%d\n",(int)nodeId);
    for(int idx = 0; idx < graph.adjList_[nodeId].size(); ++idx)
    {
    	if(!check[graph.adjList_[nodeId][idx]])
    	{    		
    		check[graph.adjList_[nodeId][idx]] = true;
    		DFS(graph, graph.adjList_[nodeId][idx], check);
    	}
   }
}

//=========================================
// shortest path
//=========================================
class NodeDistCmp
{
	public:
		NodeDistCmp(const std::vector<int>& refDepot)
		:depot_(refDepot)
		{};
		bool operator()(const IdType& a, const IdType& b) const
		{
			return (depot_[a] < depot_[b]);
		};
	private:
	    const std::vector<int>& depot_;	
};

template<typename G>
void shortestPath(G& graph, const IdType srcNodeId)
{
	//For inspection
	std::vector<IdType> ans(graph.nodeDepot_.size(), INT_MAX);
	ans[srcNodeId] = srcNodeId;
	
	//IdType: the end Node / Where it is in the std::vector<int>
    //typedef std::pair<IdType>  NodeDist;    
    std::priority_queue<NodeDistCmp, std::vector<IdType> > pq;
    std::set<IdType> checkHT;
    
    //initialization
    std::vector<int> nodeDistVec(graph.nodeDepot_.size(), INT_MAX);
    pq.push(srcNodeId);
    while(!pq.empty())
    {
    	IdType currNodeId = pq.top();
    	pq.pop();
    	
		//relaxation of end nodes
		//and push the newly updated node/dist into pq    
    	for(int idx = 0; idx < graph.adjList_[currNodeId].size() ; ++idx)
		{
			IdType toNodeId = graph.adjList_[currNodeId][idx];
			//if( checkHT.end() != checkHT.find(toNodeId) )
    	    //    continue;
						
			if( nodeDistVec[toNodeId] > 
			    nodeDistVec[currNodeId] + 
				graph.edgeDepot_[graph.neMap_[currNodeId][toNodeId]].weight_ )
			{
				nodeDistVec[toNodeId] = 
				nodeDistVec[currNodeId] + 
				graph.edgeDepot_[graph.neMap_[currNodeId][toNodeId]].weight_;
				pq.push(toNodeId);
				ans[toNodeId] = currNodeId;				
			}										    
		}    
    }
    
    printf("================== shortest Path================\n");
    IdType tmpNodeId = IdType(5);
    printf("==>%d\n", tmpNodeId.val());
    //for(int idx = 0; idx < ans.size(); ++idx)
    while( !(!(tmpNodeId < srcNodeId) && !(tmpNodeId > srcNodeId)) )
    {
    	printf("==>%d\n", ans[tmpNodeId].val());
    	tmpNodeId = ans[tmpNodeId];
    }
}

//===================================
// Longest path
//===================================
//
// Get list of backward edge list. Also practice generic praogramming 
// 
//
	struct Rec
	{
	    IdType nodeVId;	
		std::vector<IdType>::iterator iter;
	};
template<typename G, typename B>
void getBackEdgeList(G& graph, B& backEdgeListInserter)
{
	#if 0 //this is not allowed in C++98
	struct Rec
	{
	    IdType nodeVId;
		IdType nodeWId;	
		typename G::listType::iterator iter;
	};
	#endif
	
	Rec curRec;
	int cnter = 0; //default starting number of node numbering system
	std::stack<Rec> st;
	std::map<IdType,bool> predecessorHT;
	std::vector<bool> checked(graph.adjList_.size(), false);
	for( typename G::adjListType::iterator iter = graph.adjList_.begin() ;
	     iter != graph.adjList_.end();
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
		    if( curRec.iter != graph.adjList_[curRec.nodeVId].end())
		    {	
			     if( checked[*curRec.iter] )
				 {				  
		             if( (predecessorHT.end() != predecessorHT.find(*curRec.iter) && predecessorHT[*curRec.iter]) 			     
			           )    
		             {
		     	        //backEdgeList.push_back( graph.neMap_[curRec.nodeVId][*curRec.iter] );
						backEdgeListInserter( graph.neMap_[curRec.nodeVId][*curRec.iter] );		     	    
		             }
		             ++curRec.iter;
		         }
				 else
				 {				 				    
				    //DFS				    
				    predecessorHT[curRec.nodeVId] = true;
				    checked[curRec.nodeVId]       = true;
				    st.push(curRec);
				    curRec.nodeVId = (*curRec.iter)	;
				    curRec.iter    = graph.adjList_[curRec.nodeVId].begin();
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
	for(typename B::iterator iter = backEdgeListInserter.getContainer().begin();
	    iter != backEdgeListInserter.getContainer().end();
	    ++iter
	   )
	{
		printf("Directed Back Edge: (%d, %d)\n", graph.edgeDepot_[*iter].fromId_.val(), graph.edgeDepot_[*iter].toId_.val());
	}
}


template<typename G>
void dagLogestPath(G& network, IdType source, std::vector<int>& dist, std::set<IdType> skipEdge, std::map<IdType,IdType>& ans)
{
	//
	// Precondition:
	// network contains ONLY one source and one sink
	// it should be an acyclic network......
	//
	// NOTE: distance for every nodes is saved in one container.
	//       If the implementation take dist as an input parameter,
	//       then it is user's responsibility to initialize it correctly
	//       and maintain it when it is out out the scope of this function.
	//
	if( network.adjList_.size() <= 0 ) return;
	
	//std::vector<IdType> mapping(network.adjList_.size(), network.adjList_.size());
	std::vector<int> inDegree(network.adjList_.size());
	//initialize indegree;
	for(int idx = 0; idx < inDegree.size(); ++idx)
	{
		inDegree[idx] = network.nodeDepot_[idx].indegree_;
	}
	// Minus the inDegrees in the skip edges
	for(std::set<IdType>::iterator iter = skipEdge.begin();
	    iter != skipEdge.end();
	    ++iter
	   )
	{
		IdType toId = network.edgeDepot_[(*iter)].toId_;		
		--inDegree[toId];
	}
	
	//Please read the NOTE at the beginning of this function // it is changed to be an input parameter
	//std::vector<int> dist(network.adjList_.size(), INT_MIN);	
	//dist[source] = 0;
	
	// curNode --- parentNode in the path
	// it is changed to be an input parameter
	//std::map<IdType,IdType> ans; 
	std::queue<IdType> queue;		
	IdType curNodeId;
	queue.push(source);	
	while(!queue.empty())
	{
		curNodeId = queue.front();
		queue.pop();
		
		//printf("now checking:%d\n", curNodeId.val());
		for(typename G::listType::const_iterator cIter = network.adjList_[curNodeId].begin();
		    cIter != network.adjList_[curNodeId].end();
		    ++cIter
		   )
		{			
			//skip some unwanted edges, possibly backward edges
			if ( skipEdge.end() != skipEdge.find(network.neMap_[curNodeId][*cIter]) ) continue;
			
			//relaxation
			if( dist[*cIter] < 
			    dist[curNodeId] + network.edgeDepot_[network.neMap_[curNodeId][*cIter]].weight_ )
			{
				dist[*cIter] = dist[curNodeId] + network.edgeDepot_[network.neMap_[curNodeId][*cIter]].weight_;			
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
    IdType slow = curNodeId;
    IdType fast = ( ans.end() != ans.find(slow) ) ? ans[slow] : curNodeId;
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
	//curNodeId = IdType(5);	
	//printf("Node: %d\n", curNodeId.val());
    if( !(slow == fast)	)
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
bool longestPath(G& network, IdType source, std::map<IdType,IdType>& ans)
{
	if(network.adjList_.size() <= 0 ) return false;
	
	//===========================================================
    //first separate the edges in network to backages and forward edges
    //===========================================================
    std::set<IdType> backEdgeSet;
    Inserter<std::set<IdType> > inserter(backEdgeSet);
	getBackEdgeList(network, inserter);

	//================================================
	// start to find the longest path
	//================================================
	std::vector<int> dist(network.adjList_.size(), INT_MIN); // intermediate dist
	dist[source] = 0;
	int count = 0;
	bool bRelaxed = false;
	do
	{
	    bRelaxed = false;	
	    dagLogestPath(network, source, dist, backEdgeSet, ans);
	    
	    for(std::set<IdType>::iterator iter = backEdgeSet.begin();
	        iter != backEdgeSet.end();
	        ++iter
		   )
	    {
	    	if(dist[network.edgeDepot_[*iter].toId_ ]  < 
	    	   dist[network.edgeDepot_[*iter].fromId_] +
	    	   network.edgeDepot_[*iter].weight_
			  )
	    	{
	    	    dist[network.edgeDepot_[*iter].toId_ ]  = 
	    	    dist[network.edgeDepot_[*iter].fromId_]  +
	    	    network.edgeDepot_[*iter].weight_;
	    	    bRelaxed = true;
	    	    ans[network.edgeDepot_[*iter].toId_] = network.edgeDepot_[*iter].fromId_;
	    	}	    	
	    }
	    ++count;
	    if( count > backEdgeSet.size() && bRelaxed )
	        break;
    } while(bRelaxed);
#if 0    
    IdType curNodeId = 5;
    while( ans.end() != ans.find(curNodeId))
	{		
		printf("Cyclic Node: %d\n", curNodeId.val());
		curNodeId = ans[curNodeId];
	}
	printf("Cyclic Node: %d\n", curNodeId.val());
#endif
for(std::map<IdType,IdType>::iterator iter=ans.begin();
    iter != ans.end();
    ++iter
   )
{
	printf("cyclic: %d %d\n", (*iter).first.val(), (*iter).second.val());
}	
	printf("Positive Cycle:%d\n",count > backEdgeSet.size() && bRelaxed);
    return (count > backEdgeSet.size() && bRelaxed);
}
*/

namespace {
struct MyEdge 
{
    MyEdge(int weight) { weight_ = weight; };
    int weight_;
};
}

GraphLib::Graph<int,MyEdge> backEdgeTest1()
{
    //   1    1    1    1    1    1
    // 0 -> 1 -> 2 -> 3 -> 4 -> 5
    //      ^    ^    ^____|
    //      |    |_________|      
    //      |--------------|
	GraphLib::Graph<int,MyEdge> graph;
	GraphLib::IdType from = graph.addNode(0);
	GraphLib::IdType to ;
	for(int idx = 1; idx < 6; ++idx)
	{
		to = graph.addNode(idx);
		graph.addEdge(from,to,1);
		from = to;
	}    
	
	//add cycle here, here the method depends on the implementation of graph
    graph.addEdge(GraphLib::IdType(4),GraphLib::IdType(3),1);
    graph.addEdge(GraphLib::IdType(4),GraphLib::IdType(2),1);
    graph.addEdge(GraphLib::IdType(4),GraphLib::IdType(1),1);
    
    return graph;
}

GraphLib::Graph<int,MyEdge> backEdgeTest2()
{
    //   1    1    
    // 0 -> 1 -> 2 -------------- 
    //                          |
    //                          V
    // 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> 11 
    //      ^------------------------|
    //
	//NOT SINGLE SOURCE AND NOT SINGLE SINK
	GraphLib::Graph<int,MyEdge> graph;
	GraphLib::IdType from = graph.addNode(0);
	GraphLib::IdType to ;
	for(int idx = 1; idx < 3; ++idx)
	{
		to = graph.addNode(idx);
		graph.addEdge(from,to,1);
		from = to;
	}    
	
	for(int idx = 3; idx < 6; ++idx)
	{
		from = graph.addNode(idx);
		graph.addEdge(from, to,1);
		to = from;
	}	
	
	from = graph.addNode(6);
	for(int idx = 7; idx < 12; ++idx)
	{
		to = graph.addNode(idx);
		graph.addEdge(from,to,1);
		from = to;
	}
	
	//add cycle here, here the method depends on the implementation of graph
	graph.addEdge(GraphLib::IdType(2),GraphLib::IdType(8),1);
	graph.addEdge(GraphLib::IdType(9),GraphLib::IdType(4),1);
	
	return graph;
}

GraphLib::Graph<int,MyEdge> cyclicLongestGraphTest()
{
	//SINGLE SOURCE AND NOT SINGLE SINK
	//Use brutal force(depending on the implementation) to setup the graph which means
	//this is not a usual way to construct a graph in client code
	GraphLib::Graph<int,MyEdge> graph;	
	for(int idx = 0; idx < 6; ++idx)
	{
		graph.addNode(idx);
	}   
	
	GraphLib::IdType from(0) ;
	GraphLib::IdType to(1) ;
	graph.addEdge(from,to,1); 
	to = 2;
	graph.addEdge(from,to,5); 
	
	from = 2 , to = 3;
	graph.addEdge(from,to,1);
	to = 4;
	graph.addEdge(from,to,1);
	to = 1;
	graph.addEdge(from,to,-3);   
	
	from = 1, to = 2;
	graph.addEdge(from,to,2); 
	to = 5;
	graph.addEdge(from,to,2);
	
	from = 4, to = 3;
	graph.addEdge(from,to,-1);
	to = 5;
	graph.addEdge(from,to,-4);
	
	from = 5, to = 4;
	graph.addEdge(from,to,4);    
		
	return graph;
}

int main()
{
//	_IdType a = 1;
//my b;
	//printf("%ld\n",sizeof(size_t));
	GraphLib::Graph<int,MyEdge> graph;

	GraphLib::IdType srcNodeId  = graph.addNode(0);
	GraphLib::IdType fromNodeId = srcNodeId;
/*
	for(int idx = 1; idx < 6; ++idx)
	{
		IdType toNodeId = graph.addNode(idx);
		if( idx != 5)
		graph.addEdge(fromNodeId, toNodeId, 1);	
		else
		graph.addEdge(fromNodeId, toNodeId, 100);	
		
		fromNodeId = toNodeId;
	}
	fromNodeId = srcNodeId;
	for(int idx = 6; idx < 15; ++idx)
	{
		IdType toNodeId = graph.addNode(idx);
		graph.addEdge(fromNodeId, toNodeId,1);
		fromNodeId = toNodeId;
	}
	graph.addEdge(IdType(14),IdType(5),1);
	graph.addEdge(IdType(4), IdType(7),1);
	
	adjListPrint(graph);
	std::vector<bool> check(graph.nodeDepot_.size(),false);
	DFS(graph, srcNodeId, check);
	
	shortestPath(graph, srcNodeId);
*/	
	printf("=================back edge==============\n");
	std::vector<GraphLib::IdType> backEdgeList;
	Inserter<std::vector<GraphLib::IdType> > inserter(backEdgeList);
	GraphLib::Graph<int,MyEdge> testGraph = backEdgeTest1();
	GraphUtil::getBackEdgeList(testGraph, inserter);
	backEdgeList.clear();
	testGraph = backEdgeTest2();
	GraphUtil::getBackEdgeList(testGraph, inserter);
	printf("========================================\n");
	
	printf("=================dag longest path==============\n");
	std::map<GraphLib::IdType,GraphLib::IdType> ans;
	std::vector<int> dist(graph.nodeCount(), INT_MIN);
	std::set<GraphLib::IdType> skipEdge;
	dist[srcNodeId] = 0;
	GraphUtil::dagLogestPath(graph, srcNodeId, dist, skipEdge, ans);
	printf("===============================\n");
	
	printf("=================cyclic longest path==============\n");
	ans.clear();
	GraphLib::Graph<int,MyEdge> testGraph3 = backEdgeTest1();
	GraphUtil::longestPath(testGraph3, GraphLib::IdType(0), ans);
	printf("The sceond test\n");
	ans.clear();
	GraphLib::Graph<int,MyEdge> testGraph4 = cyclicLongestGraphTest();
	GraphUtil::longestPath(testGraph4, GraphLib::IdType(0), ans);
	printf("========================================\n");
	
	return EXIT_SUCCESS;
}
