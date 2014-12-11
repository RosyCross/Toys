#include "intervalTree.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <stack>
#include <utility>
#include <algorithm>

#include "baseTypes.h"
#include "geomObj.h"

//======================================
// Basic debug and verifying functions
//======================================
//==============================================================
//==============================================================
void ItvTreeUtil::inorderPrint(const Node* node)
{
    if(NULL == node) return;    
    
    inorderPrint(node->leftPtr_);
    
    printf("Node Info: count:%d coord1:%d coord2:%d status:%d\n",
            node->count_,node->coord1_, node->coord2_, node->status_            
          );
    
    inorderPrint(node->rightPtr_);
}
//==============================================================
void ItvTreeUtil::collectSeg(const Node* node, std::vector<Segment>& segVec )
{
    if( NULL == node ) return;
    
    collectSeg(node->leftPtr_,segVec);
    
    if( 0 < node->count_ )
    {
        //assume it is 1d
        if ( 0 >= segVec.size() || segVec.back().getTail().getX() != node->coord1_ )
        {       
            Segment tmpSeg = { {node->coord1_, 0}, {node->coord2_,0} };
            segVec.push_back(tmpSeg);
        }
        else
        {
            //segVec.back().getTail().getX() = node->coord2_;
            segVec.back().setTailX(node->coord2_);
        }
    }
        
    collectSeg(node->rightPtr_,segVec);
}
//==============================================================
bool ItvTreeUtil::checkInsert(const Node* root)
{
    std::vector<Segment> segVec;
    collectSeg(root,segVec);
    
    if( NULL == root || 0 == segVec.size() ) 
        printf("ERROR\n");
    else if( 1< segVec.size() )
    {
        bool rtv = true;
        IntType left  = segVec.front().getHead().getX();
        IntType right = segVec.front().getTail().getX();
        for(int idx = 1; idx < segVec.size();++idx)
        {
            if( segVec[idx-1].getTail().getX() != segVec[idx].getHead().getX() )
            {
                rtv = false;
                printf("DISJOINED interval: (%d %d)---(%d %d)\n",
                        segVec[idx-1].getHead().getX(), segVec[idx-1].getTail().getX(),
                        segVec[idx].getHead().getX(), segVec[idx].getTail().getX()
                      );
            }
            else
            {
                right = segVec[idx].getTail().getX();
            }
        }
        if(rtv)
        {
            printf("FORMED INTO SINGLE: %d %d\n", left,right);
        }
        else
        {
            printf("INTERVALS ARE DISJONED\n");
        }
    }
    else
    {
        printf("SINGLE\n");
    }    

    return true;
}

//==============================================================
void IntervalTree::destroyTreeNode(Node* node)
{
    if( NULL == node ) return;
    
    destroyTreeNode(node->leftPtr_); 
    node->leftPtr_ = NULL;

    destroyTreeNode(node->rightPtr_);
    node->rightPtr_ = NULL;
    
    delete node;    
}
//==============================================================
Node::Status IntervalTree::updateNodeStatus(Node* node)
{
    if( NULL == node ) return Node::ERROR;
    
    if( node->count_ > 0 )
    {   
        node->status_ = Node::FULL;
    }
    else
    {
        if(NULL == node->leftPtr_ && NULL == node->rightPtr_ )
        {
            node->status_ = Node::EMPTY;
        }
        else
        {
            if( ( NULL == node->leftPtr_ ||
                 (NULL != node->leftPtr_  && Node::EMPTY == node->leftPtr_->status_) 
                ) 
                &&
                ( NULL ==  node->rightPtr_||
                 (NULL != node->rightPtr_ && Node::EMPTY == node->rightPtr_->status_)
                ) 
              )
                node->status_ = Node::EMPTY;     
            else  
                node->status_ = Node::PARTIAL;     
        }     
    }
    
    return node->status_;
}
//==============================================================
void IntervalTree::insert(IntType lower, IntType upper)
{
    if ( lower == upper ) return;   
    
    iInsert(lower,upper,root_);
}
//==============================================================
void IntervalTree::remove(IntType lower, IntType upper)
{
    if ( lower == upper ) return;   
    
    iDelete(lower,upper,root_);
}
//==============================================================
Node::Status IntervalTree::iInsert(IntType lower, IntType upper, Node* node)
{
    if ( lower == upper || NULL == node ) return Node::ERROR;   
    
    if( lower <= node->coord1_ && node->coord2_ <= upper /*&& (NULL == node->rightPtr_ && NULL == node->leftPtr_)*/)
    {
        node->count_ += 1;
    }   
    else if(  node->coord1_ < upper && lower < node->coord2_ )
    {
        //this line could be overflow or under flow
        int midCoord = (node->coord1_+node->coord2_)/2;
        
        if( midCoord < upper )
        {
            if( NULL == node->rightPtr_ )
            {           
                node->rightPtr_ = new Node;
                node->rightPtr_->coord1_ = midCoord;
                node->rightPtr_->coord2_ = node->coord2_;
                //node->rightPtr_->count_  = node->count_;
            }
                
            iInsert(lower, upper, node->rightPtr_); 
        }
        
        if( lower < midCoord )
        {
            if( NULL == node->leftPtr_ )
            {
                node->leftPtr_ = new Node;
                node->leftPtr_->coord1_ = node->coord1_;
                node->leftPtr_->coord2_ = midCoord;
                //node->leftPtr_->count_  = node->count_;
            }
            
            iInsert(lower, upper, node->leftPtr_);  
        }
    }   
    
    return updateNodeStatus(node);
}
//==============================================================
Node::Status IntervalTree::iDelete(IntType lower, IntType upper, Node* node)
{
    if ( lower == upper || NULL == node ) return Node::ERROR;
    
    if( lower <= node->coord1_ && node->coord2_ <= upper && Node::FULL == node->status_)
    {
        node->count_ -= 1;      
        assert(node->count_ >= 0 );     
    }   
    else if(  node->coord1_ < upper && lower < node->coord2_ )
    {   //Beware of the case:
        //Sometimes, the node is in full state meaning that the left/right child is possibly NULL. 
        //the following deletion action could be only a part of the segment in full state.
        //So in this situation, split and generate left/right child in order to do the following
        //deletion. If this is not done, the deletion procedure stops because of the left/right
        //child is NULL, since its parent is in full state.
        
        //this line could be overflow or under flow
        int midCoord = (node->coord1_+node->coord2_)/2;     
        bool notFullAnymore = false;            
        if( midCoord < upper && node->rightPtr_ )
        {                                                   
            if( Node::EMPTY == iDelete(lower, upper, node->rightPtr_) )
            {           
                destroyTreeNode(node->rightPtr_);
                node->rightPtr_ = NULL;
                notFullAnymore  = true;
            }
        }
        
        if( lower < midCoord && node->leftPtr_)
        {                              
            if( Node::EMPTY == iDelete(lower, upper, node->leftPtr_) )
            {
                destroyTreeNode(node->leftPtr_);    
                node->leftPtr_ = NULL;
                notFullAnymore  = true;
            }
        }
    }
    return updateNodeStatus(node);
}   
//==============================================================
void IntervalTree::collectSegment(Point head, Point tail,std::vector<Segment>& segVec)
{
    if( 0 >= segVec.size() || 
        (segVec.back().getTail().getY() != head.getY() ) )
    {
        Segment tmpSeg = { head, tail };
        segVec.push_back( tmpSeg );
    }
    else // can merge the previous segment with the current segment
    {
        //segVec.back().getTail().getY() = tail.getY();
        segVec.back().setTailY(tail.getY());
    }
}
void IntervalTree::iReverseOverlap(IntType lower,IntType upper, Node* node, std::vector<Segment>& segVec)
{
    if( upper -lower < 1 || NULL == node ) return;
    if( Node::FULL == node->status_ ) return;
        
    if(  lower <= node->coord1_ &&  node->coord2_ <= upper &&
         Node::EMPTY == node->status_ 
      )
    {
        //Point head = {0,node->coord1_}, tail={0,node->coord2_};
        collectSegment(Point{0,node->coord1_},Point{0,node->coord2_},segVec);       
    }
    else if( Node::EMPTY == node->status_ )
    {
        //collectSegment(Point{0,lower},Point{0,upper},segVec);
        collectSegment(Point{0,std::max(lower,node->coord1_)},Point{0,std::min(node->coord2_,upper)},segVec);
        //collectSegment(Point{0,node->coord1_},Point{0,node->coord2_},segVec);
    }
    else if( Node::PARTIAL == node->status_ )
    {
        int mid = (node->coord2_ + node->coord1_ ) / 2;
        //must go left first
        if ( node->coord2_ > lower && upper > node->coord1_ && lower < mid )
        {
            if ( NULL != node->leftPtr_ )
                iReverseOverlap(lower, upper, node->leftPtr_, segVec);
            else //Beware!!!!!
                collectSegment(Point{0,std::max(lower,node->coord1_)},Point{0,std::min(mid,upper)},segVec);
                //collectSegment(Point{0,std::max(lower,node->coord1_)},Point{0,mid},segVec);    
        }
        
        if ( node->coord2_ > lower && upper > node->coord1_ && mid < upper )
        {
            if( NULL!= node->rightPtr_)
                iReverseOverlap(lower, upper, node->rightPtr_, segVec);
            else //Beware!!!!!
                collectSegment(Point{0,std::max(mid,lower)},Point{0,std::min(upper,node->coord2_)},segVec);
                //collectSegment(Point{0,mid},Point{0,std::min(upper,node->coord2_)},segVec);
        }
    }
}
//==============================================================
void IntervalTree::reverseOverlap(IntType lower,IntType upper, std::vector<Segment>& segVec)
{
    if( upper - lower < 1 ) return;
    
    iReverseOverlap(lower,upper, root_, segVec);
}


