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
#include <queue>
#include <deque>

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

        bool operator<(size_t rhs) const {return idVal_  < rhs;};
        bool operator>(size_t rhs) const {return idVal_  > rhs;};
        bool operator==(size_t rhs) const {return idVal_ == rhs;};     
        bool operator>=(size_t rhs) const {return idVal_ >= rhs;};
        
        //may not be necessary
        const _IdType& operator=(int newVal )  { idVal_ = newVal; return *this;};
        int val() const {return idVal_;}
        
    private:
        int idVal_;
};
//const _IdType MAX_IDVAL(INT_MAX);
//const _IdType MIN_IDVAL(INT_MIN);
typedef _IdType NodeIdType;
typedef _IdType EdgeIdType;
typedef _IdType IdType;

//typedef int IdType;
template<typename T>
class GraphNode
{
    public:
    GraphNode(const T& userData): userData_(userData), indegree_(0), id_(INT_MIN){};
    
        
    T userData_;
    int indegree_;
    NodeIdType id_; 
};

template <typename T>
class GraphEdge
{
    public:
    GraphEdge(IdType from,IdType to,const T& userDataIn):fromId_(from),toId_(to), userData_(userDataIn) {};
    IdType getId() const { return edgeId_; }        

    IdType fromId_;
    IdType toId_;
         T userData_;
    EdgeIdType edgeId_;
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

        typedef std::vector<IdType> IdListType;
        typedef std::vector<IdListType> AdjListType;
        
        typedef std::vector< GraphNode<T> >        NodeDepotType;
        typedef std::vector<GraphEdge<S> >         EdgeDepotType;
        
        typedef IdListType::iterator       IdIter; 
        typedef IdListType::const_iterator const_IdIter;
        
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
        //templated addEdge functions
        IdType addNode(const T& userData);
        IdType addEdge(IdType from, IdType to,const S& edgeData);
        IdType addEdge(const T& userDataFrom, const T& userDataTo, const S& edgeData);
        IdType addEdge(IdType from, const T& userDataTo, const S& edgeData);

        //remove node/edge functions. Would be complicated with vector or deque
        void removeNode(const IdType& nodeId);        
        void removeEdge(const IdType& edgeId);



        //==============
        //Get Count
        //==============
        size_t nodeCount() { return nodeDepot_.size(); }
        size_t edgeCount() { return edgeDepot_.size(); }
        

        //=====================
        //Get data from adjacent list, beware that there is NO validity checking
        //=====================
        //Experimental...should not use it unless you are sure what you are doing 
        IdType getNodeIdByIdx(size_t idx)
        {
            return IdType(nodeDepot_[idx].id_);
        }
        
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
        { 
            return nodeIdMapper_.count(nodeId)>0; 
        }
        
        bool isValidEdgeId(IdType edgeId) const
        { 
            return edgeIdMapper_.cout(edgeId)>0; 
        }
        
        //=======================
        //Get things from Depot
        //=======================
        //
        // Fundamental getter with sanity checking
        const NodeType* getNode(IdType nodeId) const
        {
            IdType id = nodeIdMapper_.getId(nodeId);
            if (id >= nodeDepot_.size()) return NULL;

            return &nodeDepot_[id];
        }

        const EdgeType* getEdge(IdType edgeId) const
        {
            IdType id = edgeIdMapper_.getId(edgeId);
            if (id >= edgeDepot_.size()) return NULL;

            return &edgeDepot_[id];
        }
        
        NodeType* getNode(IdType nodeId) 
        {
            return
            const_cast<NodeType*>(
                const_cast<const Graph*>(this)->
                    getNode(nodeId));
        }

        EdgeType* getEdge(IdType edgeId)
        {
            return
            const_cast<EdgeType*>(
                const_cast<const Graph*>(this)->
                    getEdge(edgeId));
        }

        const NodeType& getNodeData(IdType nodeId) const
        { 
            const NodeType* node = getNode(nodeId);
            assert(node);
            return *node; 
        };
        
        const EdgeType& getEdgeData(IdType edgeId) const
        { 
            const EdgeType* edge = getEdge(edgeId);
            assert(edge);
            return *edge; 
        };

        NodeType& getNodeData(IdType nodeId)
        { 
            return
            const_cast<NodeType&>(
                const_cast<const Graph*>(this)->
                    getNodeData(nodeId));
        };
        
        EdgeType& getEdgeData(IdType edgeId)
        {
            return
            const_cast<EdgeType&>(
                 const_cast<const Graph*>(this)->
                      getEdgeData(edgeId) );
        }
        
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
        AdjListType adjList_;
        //cannot think of a concise model for this
        std::map<IdType, std::map<IdType, IdType> > neMap_; 
        //depots
        NodeDepotType nodeDepot_;
        EdgeDepotType edgeDepot_;  
    
    private:
        //IdType valueToId(size_t value) {return (IdType)value;}          
        class IdMapper;
        template<typename Cntr>
        void removeDepotId(const IdType& id, IdMapper& idMapper,Cntr& depot);
        IdMapper nodeIdMapper_;
        IdMapper edgeIdMapper_;
};

template<typename T, typename S>
IdType Graph<T,S>::addNode(const T& userData)
{
    IdType id = nodeIdMapper_.genId(nodeDepot_);
    if (id == nodeDepot_.size())
        nodeDepot_.push_back(GraphNode<T>(userData));
    else
        nodeDepot_[id].userData_ = userData;
    nodeDepot_[id].id_ = id;
    //remove this line if adjList becomes hash
    if (id == adjList_.size())
       adjList_.push_back(IdListType());//a place holder      
    else
       adjList_[id].clear();

    nodeIdMapper_.addId(id,id);
    return id;
}
template<typename T, typename S>
void Graph<T,S>::removeNode(const IdType& nodeId)
{
    removeDepotId(nodeId, nodeIdMapper_, nodeDepot_);
}

template<typename T, typename S>
template<typename Cntr>
void Graph<T,S>::removeDepotId(const IdType& nodeId,IdMapper& idMapper,  Cntr& depot)
{
    typename IdMapper::iterator it = idMapper.find(nodeId);
    if ( idMapper.end() == it)
        return;

    if ( nodeId == depot.size()-1 )
    {
        depot.erase(depot.end()-1);
        idMapper.removeId(nodeId); 
    }
    else
    {
        IdType lastNodeId = depot.size() - 1;    
        typename IdMapper::iterator lastNodeIdMapIt = 
                                         idMapper.find(nodeId);
        //swap nodeId content to the last position
        std::swap(depot[nodeId], depot[lastNodeId]);
        //now the nodeId is the node of the orginal lastNodeId.
        //adjust all the info in the node. 
        depot[nodeId].id_ = nodeId;//might not be necessary
        (*lastNodeIdMapIt).second = nodeId;
        //remove the all related info with the intended to be removed node
        depot.erase(depot.end()-1);
        idMapper.removeId(nodeId);
        idMapper.addReuseId(nodeId);
    }

}

//================================
// edge operation
//================================
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
        return INT_MAX;
        
    IdType id = edgeIdMapper_.genId(edgeDepot_);
    if (id == edgeDepot_.size())
        edgeDepot_.push_back(GraphEdge<S>(from,to,edgeData));
    else
        edgeDepot_[id].userData_ = edgeData;

    edgeDepot_[id].fromId_   = from;
    edgeDepot_[id].toId_     = to;
    //edgeDepot_[id].back().userData_ = edgeData;
    edgeDepot_[id].edgeId_   = id;
    adjList_[from].push_back(to);
    neMap_[from][to] = id;

    ++nodeDepot_[to].indegree_;

    edgeIdMapper_.addId(id,id);
    return id;
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

template<typename T, typename S>
void Graph<T,S>::removeEdge(const IdType& edgeId)
{
    typename EdgeDepotType::iterator it = edgeDepot_.find(edgeId);
    if (it==edgeDepot_.end()) return;

    GraphEdge<S>& edge = edgeDepot_[(*it).second];
    removeNode(edge.fromId_);
    removeNode(edge.toId_);
    removeDepotId(edgeId, edgeDepot_);
}

//============================
// Id Mapper
//============================
template<typename T, typename S>
class
Graph<T,S>::IdMapper
{
    public:
        IdType getId(const IdType& id) const;
        template<typename Ctnr>
        IdType genId(const Ctnr& depot);
        void   addId(const IdType& idFrom, const IdType& idTo);
        void removeId(const IdType& id);
        void   addReuseId(const IdType& id);

    private:
        typedef std::map<IdType,IdType> IdMap;
        IdMap idMap_;
        std::priority_queue<IdType> reuseIds_;
};

template<typename T, typename S>
IdType
Graph<T,S>::IdMapper::getId(const IdType& id) const
{
    IdMap::const_iterator it =
    idMap_.find(id);
    return it != idMap_.end() ? (*it).second : IdType(INT_MAX);    
}

template<typename T, typename S>
template<typename Ctnr>
IdType
Graph<T,S>::IdMapper::genId(const Ctnr& depot)
{
    if (reuseIds_.empty())
    {
        return depot.size();
    }
    else
    {
        IdType id = reuseIds_.top();
        reuseIds_.pop();
        return id; 
    }
}

template<typename T, typename S>
void
Graph<T,S>::IdMapper::addId(const IdType& idFrom, const IdType& idTo)
{
    idMap_[idFrom] = idTo;
}

template<typename T, typename S>
void
Graph<T,S>::IdMapper::removeId(const IdType& id)
{
    idMap_.erase(id);
}

template<typename T, typename S>
void
Graph<T,S>::IdMapper::addReuseId(const IdType& id)
{
    reuseIds_.push(id);
}

} //namespace
#endif
