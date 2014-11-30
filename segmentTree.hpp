#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_


//#include <stdio.h>
//#include <stdlib.h>
#include <limits.h>
//#include <time.h>

#include <stddef.h>
#include <bits/stl_tree.h>
#include <utility>
/*
#include <algorithm>
#include <vector>
*/
namespace STD=std;

namespace
{
    typedef int SegCoord;
    //== MyCmp is used to avoid problems caused by use std::less<int> as
    //   a comparator with _Rb_tree. Still dont know why it happens.
    //   (clang++ version:3.5) 
    struct MyCmp 
    {
       bool operator()(int a, int b)const { /*printf("cmp:%d %d\n",a,b);*/ return a<b;} 
    };

    bool opEqual(int a, int b) { return a == b; }
    bool opEqualDummy(int a, int b) { return false; }
}

class SegmentTreeNode
{
    public:
        typedef void* ObjIdType;

    public:
        SegmentTreeNode():startCoord_(0),endCoord_(0),refCoord_(0),maxCoord_(INT_MIN),id_(0) 
        {};
        SegmentTreeNode(int startCoord,int endCoord,int refCoord,ObjIdType id):
        startCoord_(startCoord),endCoord_(endCoord),refCoord_(refCoord),
        maxCoord_(std::max(startCoord,endCoord)),id_(id) 
        {};

        int getStartCoord() const { return startCoord_; }
        int getEndCoord()   const { return endCoord_;   }
        int getRefCoord()   const { return refCoord_;   }
        int getMaxCoord()   const { return maxCoord_;   }
  ObjIdType getId() const {return id_;}

       void setStartCoord(int newVal) { startCoord_=newVal; }
       void setEndCoord(int newVal)   { endCoord_=newVal;   }
       void setRefCoord(int newVal)   { refCoord_=newVal;   }
       void setMaxCoord(int newVal)   { maxCoord_=newVal;   }
       void setId(ObjIdType newId) { id_=newId;}

       bool operator==(const SegmentTreeNode& rhs)
       {
           //only compares segment coordinates. skip id
           return startCoord_ == rhs.startCoord_ && endCoord_ == rhs.endCoord_;
       }

    public:
        struct Key
        {
            //must return reference in order to conform to stl_tree implementation
            const int& operator()(const SegmentTreeNode& node) const 
            { return node.startCoord_; }
        };

    private:
        int startCoord_;
        int endCoord_;
        int refCoord_;
        int maxCoord_;
        //size_t id_;
  ObjIdType id_;
};
#define CAST2NODE(baseNode) static_cast<_Link_type>(baseNode)
#define GETUD(node) node->_M_value_field //get user data
#define GETDATA(baseNode) GETUD(CAST2NODE(baseNode)) //get user data from base node type
 
//typedef std::_Rb_tree<int,int,std::_Identity<int>, std::less<int> > RBTree;
template<typename K,typename V, typename KofV, typename Comp,typename Alloc=STD::allocator<V> >
class SegmentTreeTP : public STD::_Rb_tree<K,V,KofV,Comp,Alloc>
{
    public:
        //some typedefs
        typedef STD::_Rb_tree<K,V,KofV,Comp,Alloc> BaseTree;
        typedef STD::_Rb_tree_node<V> _Rb_tree_node;
        
        using typename BaseTree::value_type;
        using typename BaseTree::reference;
        using typename BaseTree::const_reference;
        using typename BaseTree::key_type;
        using typename BaseTree::iterator;
        using typename BaseTree::_Base_ptr;
        using typename BaseTree::_Link_type;

    public:
        //insertion
        STD::pair< iterator, bool > 
        insert_unique(const value_type& value)
        {
            STD::pair<iterator, bool> tmp = _M_insert_unique(value);
            updateMeta(static_cast<_Link_type>(tmp.first._M_node));
            return tmp;
            //return _M_insert_unique(value);
        };

        iterator 
        insert_equal(const value_type& value)
        {
            iterator tmp = this->_M_insert_equal(value);
            updateMeta(static_cast<_Link_type>(tmp._M_node));
            return tmp;
        };

        template<typename F>
        void getOverlap(SegCoord x1, SegCoord x2, F functor);

        bool hasAnyOverlap(SegCoord x1, SegCoord x2)
        {
            return hasAnyOverlap(x1,x2,static_cast<_Link_type>(this->_M_root()));
        };

        //erase the first encountered same segment
        //void eraseFirst(SegCoord x1, SegCoord x2);
        void eraseFirst(const value_type& value);

        //erase all the same segment
        //void eraseAll(SegCoord x1, SegCoord x2);
        void eraseAll(const value_type& value);

        void inorder()
        {
            inorder(static_cast<_Link_type>(this->_M_root()));
        }

        bool verifyUnique()
        {
            return verify(static_cast<_Link_type>(this->_M_root()), opEqualDummy);
        }

        bool verifyEqual()
        {
            return verify(static_cast<_Link_type>(this->_M_root()), opEqual);
        }
 
    private:
        using STD::_Rb_tree<K,V,KofV,Comp,Alloc>::_M_insert_unique;

        bool hasAnyOverlap(SegCoord x1, SegCoord x2, _Link_type node);

        template<typename F>
        void getOverlap(SegCoord x1, SegCoord x2, F& functor, _Link_type node);

        void inorder( _Link_type node) 
        {
            if (NULL == node) return;

            inorder( this->_S_left( node ) );

            printf("%d %d %d\n", this->_S_value(node).getStartCoord(), this->_S_value(node).getEndCoord(),this->_S_value(node).getMaxCoord());
 
            inorder(this->_S_right(node));
            
        };

        void updateMax( _Link_type node)
        {
            if ( NULL == node ) return;

            SegCoord leftMax = 
            NULL != CAST2NODE(node->_M_left) ? 
            GETDATA(node->_M_left).getMaxCoord() : INT_MIN;

            SegCoord rightMax = 
            NULL != CAST2NODE(node->_M_right) ? 
            GETDATA(node->_M_right).getMaxCoord() : INT_MIN;
         
            GETUD(node).setMaxCoord(STD::max( GETUD(node).getEndCoord(),
                                    STD::max(leftMax,rightMax)));
        }

        void updateMeta( _Link_type node)
        {
            //=== this function updates from node to the root of the tree
            if ( NULL == node ) return;

            while(this->_M_root() != node)
            {
                updateMax(this->_S_right(node));
                updateMax(this->_S_left(node));
                node = CAST2NODE(node->_M_parent);
            }
            updateMax(this->_S_right(node));
            updateMax(this->_S_left(node));
            updateMax(node);
        }

        bool verify( _Link_type node, bool (*opFunc)(int,int) )
        {
            if ( NULL == node ) return true;

            int c1 = NULL!=node->_M_left ? GETDATA(node->_M_left).getStartCoord():INT_MIN;
            int c2 = NULL!=node->_M_right ? GETDATA(node->_M_right).getStartCoord():INT_MAX;
         
            reference data = GETUD(node);

            if (!( (data.getStartCoord() > c1 || 
                    opFunc(data.getStartCoord(),c1)) 
                   && 
                   (data.getStartCoord() < c2 || 
                    opFunc(data.getStartCoord(),c2))
                 )
               )
                {printf("start fail:%d %d = %d = key:%d\n",c1,c2,data.getStartCoord(), this->_S_key(node));return false;}

            int mc1 = NULL!=node->_M_left ? GETDATA(node->_M_left).getMaxCoord():INT_MIN;
            int mc2 = NULL!=node->_M_right? GETDATA(node->_M_right).getMaxCoord():INT_MIN;
     
            if ( data.getMaxCoord() != STD::max(data.getEndCoord(), STD::max(mc1,mc2)) )
                {printf("max fail %d %d %d = %p \n",mc1, mc2, data.getMaxCoord(),node->_M_left);return false;}

            return verify(CAST2NODE(node->_M_left), opFunc) && 
                   verify(CAST2NODE(node->_M_right), opFunc) ; 

        }
};

//================== Class Implementations ==========//
template<typename K, typename V, typename KofV, typename Comp, typename Alloc>
bool
SegmentTreeTP<K,V,KofV,Comp,Alloc>::hasAnyOverlap(SegCoord x1, SegCoord x2, _Link_type node)
{
    if ( NULL == node ) return false;

    //check if overlap current, if yes, return IMMEDIATELY
    if ( this->_S_value(node).getStartCoord() <= x2 &&
         this->_S_value(node).getEndCoord() >= x1
       ) 
        return true;

    //check if need to check left
    if ( this->_S_value(node).getMaxCoord() >= x1 &&
         hasOverlap(x1,x2,this->_S_left(node))
       )
        return true;
   
    return hasOverlap(x1,x2,this->_S_right(node));
}
template<typename K, typename V, typename KofV, typename Comp,typename Alloc>
void
SegmentTreeTP<K,V,KofV,Comp,Alloc>::eraseFirst(const typename SegmentTreeTP<K,V,KofV,Comp,Alloc>::value_type& value)
{
    STD::pair<iterator,iterator> range= this->equal_range(KofV()(value));
    for (;range.first != range.second; ++range.first)
    {
        //make sure they are totally the same.
        if ( *range.first == value )
            break;
    }

    this->erase(range.first);
}

template<typename K, typename V, typename KofV, typename Comp,typename Alloc>
void
SegmentTreeTP<K,V,KofV,Comp,Alloc>::eraseAll(const typename SegmentTreeTP<K,V,KofV,Comp,Alloc>::value_type& value)
{
    STD::pair<iterator,iterator> range= this->eqaul_range(KofV()(value));
    for (;range.first != range.second; ++range.first)
    {
        //make sure they are totally the same.
        if ( *range.first == value )
            this->erase(range.first);    
    }
}

//=====================================================//
template<typename K, typename V, typename KofV, typename Comp,typename Alloc>
template<typename F>
void
SegmentTreeTP<K,V,KofV,Comp,Alloc>::getOverlap(SegCoord x1, SegCoord x2, F functor)
{
    getOverlap(x1,x2,functor, CAST2NODE(this->_M_root()));    
}

template<typename K,typename V, typename KofV, typename Comp,typename Alloc>
template<typename F>
void
SegmentTreeTP<K,V,KofV,Comp,Alloc>::getOverlap(SegCoord x1, SegCoord x2, F& functor, _Link_type node)
{
    if ( NULL == node) return;

    if ( this->_S_value(node).getStartCoord() <= x2 && 
         this->_S_value(node).getEndCoord()   >= x1
       )
    {
        functor( GETUD(node) );
    }

    if ( NULL != this->_S_left(node) && 
         this->_S_value(node).getMaxCoord() >= x1 )
        getOverlap(x1,x2,functor, this->_S_left(node) );

    if ( NULL != this->_S_right(node) )
    getOverlap(x1,x2,functor, this->_S_right(node) );
}


typedef SegmentTreeTP<int,SegmentTreeNode,
                    SegmentTreeNode::Key,
                    /*MyCmp*/STD::less<int> > SegmentTree;

/*
void printOverlap(SegmentTreeNode& node)
{
    printf("%d %d %d\n", node.getStartCoord(), node.getEndCoord(), node.getMaxCoord());
}
*/
/*
int main(int argc, char* argv[])
{
    SegmentTreeNode node(1,2,2,0);
    SegmentTree st;
#if 0
    //st._M_insert_unique(node);
    st.insert_unique(node);
    SegmentTreeNode node2(3,5,5,0);
    SegmentTreeNode node3(8,10,10,0);
    st.insert_unique(node2);
    st.insert_unique(node3);
    st.inorder();   
    printf("valid:%d\n",st.verify());
#endif

    printf("========\n");
    //STD::vector<SegmentTreeNode> nodeVec;
    srand(time(NULL));
    for(int idx = 0; idx < 1E6; ++idx )
    {
        int start = rand() % INT_MAX;
        int end   = rand() % INT_MAX;
        if (start > end) STD::swap(start, end);

//        st.insert_unique(SegmentTreeNode(start,end,end,idx)); 
        st.insert_equal(SegmentTreeNode(start,end,end,idx)); 


        //if (!st.verify()) break;
        //nodeVec.push_back(SegmentTreeNode(start,end,end,0));
        //STD::pair<SegTree::iterator,bool> tmp =
        //st.insert_unique(nodeVec.back());
        //printf("%d %d %d == bool:%d\n", (tmp.first)->getStartCoord(), (tmp.first)->getEndCoord(), (tmp.first)->getMaxCoord(), tmp.second);
    }
    //st.inorder();
    printf("unique valid:%d => %d size:%d\n",st.verifyUnique(),st.__rb_verify(),st.size());
    printf("equal valid:%d => %d size:%d\n",st.verifyEqual(),st.__rb_verify(),st.size());

    st.getOverlap(-33,19, printOverlap);

    SegmentTree stDup(st);
    printf("Bfr. size:%d Dup size:%d\n",st.size(),stDup.size());
    for(STD::pair<SegmentTree::iterator,SegmentTree::iterator> pter(stDup.begin(),stDup.end());pter.first != pter.second;++pter.first  )
    {
        st.eraseFirst(*pter.first); 
    }
    printf("Aft. size:%d Dup size:%d\n",st.size(),stDup.size());

    return EXIT_SUCCESS;
}
*/
#endif
