#ifndef _INTERVALTREE_H
#define _INTERVALTREE_H

//==============================================
//wrap the integer type used in Interval tree.
//so that we can change it more easily in the future
//===============================================
#ifndef _GCC_LIMITS_H_
#include <limits.h>
#endif
#ifndef _GEOOBJ_H_
#include "geoObj.h"
#endif
#ifndef _BASETYPES_H_
#include "baseTypes.h"
#endif

#include <stddef.h>
#include <vector>

//=============================================
// auxiliary type 
//=============================================
typedef struct _node_
{
    enum Status { ERROR, EMPTY, PARTIAL, FULL };
    _node_() : leftPtr_(NULL), rightPtr_(NULL), count_(0), coord1_(0),coord2_(0), status_(EMPTY){}
    
    _node_* leftPtr_;
    _node_* rightPtr_;
       int  count_;
       int  coord1_;
       int  coord2_;    
    Status  status_;
} Node;

//=============================================
// Some Debug functions for the interval tree
// These functions should be used for debug only
// Strive to make the scope of these functions as
// small as possible
//=============================================
void inorderPrint(Node* node);
void collectSeg(Node* node, std::vector<Segment>& segVec );
bool checkInsert(Node* root);

//=============================================
// The interval tree is the one brought by bentley. This kind of
// tree may be confusing when people know other kinds of tree like:
// segment tree, and ithe interval tree in CLRS. Basically, they are
// different.
// Interval Tree is mostly used for Integral type of varaiable,
// e.g. integral coordinates. The lowest node of Interval Tree is
// composed of [n, n+1]. and the possible nodes is of the form:
// [n, n+k]. So it is like to have an imaginary full expand of a range
// up to the minimum difference 1. The concept of Interval Tree is
// to store the input range(or you can see it as segment) by breaking it
// into small rangs which the number of ranges is the fewer the better. 
// The implementation can be:
// 1. expand the nodes at the begining. the total number is O(2^(range)) 
// 2. dynmaically allocate necessary nodes according to the input.
//
// For other trees with similar names, they store the segments in a 
// specific way instead of breaking them into pieces. 
// So we may know from above that Interval Tree here needs a way to associate
// which segment falls in the node.
//=============================================
class IntervalTree
{
    public:
        IntervalTree(IntType lowerLmt, IntType upperLmt):lowerLimit_(lowerLmt),upperLimit_(upperLmt),root_(NULL)
        {
            root_ = new Node;
            root_->coord1_ = lowerLimit_;
            root_->coord2_ = upperLimit_;
        };
        ~IntervalTree() { destroyTreeNode(root_); root_ = NULL;}
        void clear()    { destroyTreeNode(root_->leftPtr_); destroyTreeNode(root_->rightPtr_);
                          root_->leftPtr_ = NULL; root_->rightPtr_ = NULL;
                          root_->count_ = 0; root_->status_ = Node::EMPTY;}
        
        void insert(IntType lower, IntType upper);
        void remove(IntType lower, IntType upper);
        void reverseOverlap(IntType lower,IntType upper, std::vector<Segment>& segVec);
        
        
        void print() { inorderPrint(root_); };
        
    private:        
        Node::Status updateNodeStatus(Node* node);
        
        Node::Status iInsert(IntType lower, IntType upper, Node* node); 
        Node::Status iDelete(IntType lower, IntType upper, Node* node);
        void iReverseOverlap(IntType lower,IntType upper, Node* node,
                             std::vector<Segment>& segVec);
        
             
        void destroyTreeNode(Node* node);     
        void collectSegment(Point head, Point tail,std::vector<Segment>& segVec);
        
            
        IntType lowerLimit_;
        IntType upperLimit_;
        public:         
        Node*   root_;
};

#endif
