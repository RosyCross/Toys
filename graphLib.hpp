#ifndef GRAPHLIB_H_
#define GRAPHLIB_H_

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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#include <map>
#include <vector>
//#include <tuple>

namespace GraphLib {

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
        const _IdType& operator=(int newVal )  { idVal_ = newVal; return *this;};
        int val() const {return idVal_;}
        
    private:
        int idVal_;
};

//class my : public _IdType {};
typedef _IdType IdType;

//typedef int IdType;
template<typename T>
class GraphNode
{
    public:
    GraphNode(const T& userData): userData_(userData), indegree_(0), id_(INT_MIN){};
    
        
    T userData_;
    int indegree_;
    IdType id_; 
};

template <typename T>
class GraphEdge
{
    public:
    GraphEdge(IdType from,IdType to,const T& userDataIn):fromId_(from),toId_(to), userData_(userDataIn) {};
        
    IdType fromId_;
    IdType toId_;
         T userData_;
};

#if 0
class GraphEdge
{
    public:
    GraphEdge(IdType from,IdType to):fromId_(from),toId_(to), weight_(0) {};
        
    IdType fromId_;
    IdType toId_;
    int weight_;
};
#endif

template<typename T, typename S>
class Graph
{
    public: // auxiliary typedefs for convenience
        typedef GraphNode<T> NodeType;
        typedef GraphEdge<S> EdgeType;

        typedef std::vector< std::vector<IdType> > AdjListType;
        typedef std::vector< IdType >              ListType; 
        
        typedef std::vector< GraphNode<T> >        NodeDepotType;
        typedef std::vector<GraphEdge<S> >         EdgeDepotType;
        
        typedef std::vector<IdType>::iterator       IdIter; 
        typedef std::vector<IdType>::const_iterator const_IdIter;
        
        typedef std::map<IdType, std::map<IdType, IdType> > NeMapType;
    public:
        //========== section for iterators ============//
        class EdgeIdGen
        {
            public:
                EdgeIdGen(Graph& graph):
                neMap_(graph.neMap_),
                iter_(graph.neMap_.begin()) 
                {
                    if(!isDone())
                        iter2_ = (*iter_).second.begin();
                };
                IdType getNext()
                {
                    IdType edgeId;
                    if (!isDone())
                        edgeId = (*iter2_).second;
  
                    goNext();
                    return edgeId;
                }
                bool hasNext()
                {
                    return !isDone();
                }

                void init()
                {
                    iter_  = neMap_.begin();
                    if(!isDone())
                        iter2_ = (*iter_).second.begin();
                }
             private:
                bool goNext() const
                {   ++iter2_;
                    if( (*iter_).second.end() == iter2_ )
                    {
                        ++iter_; iter2_ = (*iter_).second.begin();
                    }
                    return !isDone();
                } ;
                bool  isDone() const 
                { 
                    return (neMap_.end() == iter_); 
                };

                NeMapType& neMap_;
                mutable NeMapType::iterator iter_;
                mutable NeMapType::mapped_type::iterator iter2_;
        };
        class NodeIdGen
        {
            public:
                NodeIdGen(const Graph& graph):nodeDepot_(graph.nodeDepot_),iter_(graph.nodeDepot_.begin()) {};
                bool  goNext() const { ++iter_; return !isDone(); } ;
                bool  isDone() const { return nodeDepot_.end() == iter_; };
                const IdType& getCurrent() const
                { return (*iter_).id_;};
            private:
                NodeDepotType& nodeDepot_;
                mutable typename NodeDepotType::iterator iter_;
        };    
    public:
        IdType addNode(const T& userData);
        //IdType addEdge(IdType from, IdType to,int weight);
        //IdType addEdge(const T& userDataFrom, const T& userDataTo, int weight);
        //IdType addEdge(IdType from, const T& userDataTo, int weight);
        
        //templated addEdge functions
        IdType addEdge(IdType from, IdType to,const S& edgeData);
        IdType addEdge(const T& userDataFrom, const T& userDataTo, const S& edgeData);
        IdType addEdge(IdType from, const T& userDataTo, const S& edgeData);
        
        //==============
        //Get Count
        //==============
        size_t nodeCount() { return nodeDepot_.size(); }
        size_t edgeCount() { return edgeDepot_.size(); }
        
        //size_t getOutEdgeCount(IdType from) { return adjList_[from].size(); }

        //=====================
        //Get data from adjacent list, beware that there is NO validity checking
        //=====================
        const GraphNode<T>& getNodeId(IdType from, IdType to) const 
        { return adjList_[from][to]; };
        
        const_IdIter  beginNodeIter(IdType from) const
        { return adjList_[from].begin(); }

        IdIter   beginNodeIter(IdType from)
        { return adjList_[from].begin(); } 
        
        const_IdIter  endNodeIter(IdType from) const
        { return adjList_[from].end(); }

        IdIter   endNodeIter(IdType from)
        { return adjList_[from].end(); }

                
        IdType getEdgeId(IdType from, IdType to) // map has no const version of "[]", so 
        { return neMap_[from][to];}              // cannot be a const member function here.
        
        AdjListType::const_iterator  beginNodeIter() const
        { return adjList_.begin(); }

        AdjListType::iterator  beginNodeIter()
        { return adjList_.begin(); }
        
        AdjListType::const_iterator  endNodeIter() const
        { return adjList_.end(); }

        AdjListType::iterator  endNodeIter()
        { return adjList_.end(); }
        
        NeMapType::const_iterator beginNeMapIter()  const
        {
            return neMap_.begin();
        }
        
        NeMapType::const_iterator endNeMapIter()  const
        {
            return neMap_.end();
        }
        
        //=====================
        //simple Id validity checking
        //=====================
        bool isValidNodeId(IdType nodeId) const
        { return 0 <= nodeId.val() && nodeId.val() < nodeDepot_.size(); }
        
        bool isValidEdgeId(IdType edgeId) const
        { return edgeId >= 0 && edgeId < edgeDepot_.size(); }
        
        //Get things from Depot
        GraphNode<T>& getNodeData(IdType nodeId)
        { return nodeDepot_[nodeId]; };
        
        GraphEdge<S>& getEdgeData(IdType edgeId)
        { return edgeDepot_[edgeId]; };
        
        const GraphNode<T>& getNodeData(IdType nodeId) const
        { return nodeDepot_[nodeId]; };
        
        const GraphEdge<S>& getEdgeData(IdType edgeId) const
        { return edgeDepot_[edgeId]; };
                 
        bool hasEdge(IdType from, IdType to)
        {
            if( neMap_.end() != neMap_.find(from) )
            {
                if(neMap_[from].end() != neMap_[from].find(to) )
                {
                    return true;
                }
            }
            
            return false;
        }
                 
        private: 
        //use Hash will be good. But STL::MAP is not a real HASH
        std::vector< std::vector<IdType> > adjList_;
        std::map<IdType, std::map<IdType, IdType> > neMap_; //cannot think of a concise model for this
        //depots
        std::vector< GraphNode<T> > nodeDepot_;
        std::vector<GraphEdge<S> > edgeDepot_;  
    
    private:
        IdType valueToId(size_t value) {return (IdType)value;}          
};


template<typename T, typename S>
IdType Graph<T,S>::addNode(const T& userData)
{
    nodeDepot_.push_back(GraphNode<T>(userData));
    nodeDepot_.back().id_ = nodeDepot_.size()-1;
    //remove this line if adjList becomes hash
    adjList_.push_back(std::vector<IdType>());//a place holder      
    return(IdType(nodeDepot_.size()-1));
}

template<typename T, typename S>
IdType Graph<T,S>::addEdge(IdType from, IdType to, const S& edgeData)
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
        
    edgeDepot_.push_back(GraphEdge<S>(from,to,edgeData));
    edgeDepot_.back().fromId_   = from;
    edgeDepot_.back().toId_     = to;
    edgeDepot_.back().userData_ = edgeData;
    ++nodeDepot_[to].indegree_;
    //adjList[from][to] = edgeDepot_.size()-1;
    adjList_[from].push_back(to);
    neMap_[from][to] = edgeDepot_.size()-1;
    //adjList[from].push_back(edgeDepot_.size()-1);
    //printf("F-T:(%d,%d) size:%d\n",(int)from , (int)to, adjList_[from].size());
    return(IdType(edgeDepot_.size()-1));
}

template<typename T, typename S>
IdType Graph<T,S>::addEdge(const T& userDataFrom, const T& userDataTo, const S& edgeData)
{
    IdType fromId = addNode(userDataFrom);
    IdType toId   = addNode(userDataTo);
    return addEdge(fromId, toId, edgeData);
}

template<typename T, typename S>
IdType Graph<T,S>::addEdge(IdType from, const T& userDataTo, const S& edgeData)
{
    assert( from < (int)nodeDepot_.size() && 
            from >= 0 
          );

    if( !(from < (int)nodeDepot_.size() && from >= 0 ) )      
        return INT_MIN;

    IdType to = addNode(userDataTo);
    return addEdge(from,to, edgeData);        
}

} //namespace
#endif
