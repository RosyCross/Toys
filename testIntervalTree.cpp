#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include <vector>
#include <stack>
#include <utility>

#include <algorithm>
#include <iterator> // for inserter
#include <functional> // function object for member function

#include "intervalTree.h"
#include "geoObj.h"

//==============================================================
//==============================================================
bool cmpRectVertSeg(const std::pair<IntType,size_t>& pairA,const std::pair<IntType,size_t>& pairB)
{
    return pairA.first < pairB.first;
}
//==============================================================
// LIMITATION: This CANNOT hanlde that case that any two overlapped rectangles that they have left or right
//             at the same x. And also if the union of all the rectangles is not a usual polygon,
//             then the result is unpredictable
// LIMITATION: the input rectangles must be overlapped and can be formed into ONE polygon. One person
//             can preprocess the rectangles and divide them into one group of isolated rectangles
//             and another group of overlapped rectangles
void getVerticalSeg(const std::vector<Rect>& rectVec, std::vector<Segment>& vertSegVec/*, 
                          std::vector< std::pair<Segment, size_t> >& vertSegVec*/)
{
    if( 0 >= rectVec.size() ) return;
    
    //==============================================================================
    // form an array with the content of the rectangle vertical segments and to which
    // rectangle the vertical segment belongs.
    // pair: x / which rect
    //===============================================================================
    std::vector< std::pair<IntType, IntType> > rectVertSegVec;
    for(size_t idx = 0; idx < rectVec.size(); ++idx)
    {
        rectVertSegVec.push_back(std::pair<IntType,size_t>(rectVec[idx].lowerLeft_.x_, idx) );
        rectVertSegVec.push_back(std::pair<IntType,size_t>(rectVec[idx].upperRight_.x_, idx) );
    }   
    assert( 0 == (rectVertSegVec.size() & 1) );

    //=====================================     
    //pre-sort by x for Scan line method; 
    //=====================================
    std::sort(rectVertSegVec.begin(),rectVertSegVec.end(),cmpRectVertSeg);
    for(size_t idx = 0; idx< rectVertSegVec.size(); ++idx)
    {
        printf("x:%d --- (%d, %d)=(%d,%d)\n", rectVertSegVec[idx].first,
                rectVec[rectVertSegVec[idx].second].lowerLeft_.x_,
                rectVec[rectVertSegVec[idx].second].lowerLeft_.y_,
                rectVec[rectVertSegVec[idx].second].upperRight_.x_,
                rectVec[rectVertSegVec[idx].second].upperRight_.y_
              );
    }
    
    //=================================================================
    //get the vertical segments of the contour of the merged rectangles;
    //=================================================================
    //IntervalTree itvTree(rectVertSegVec.front().first, rectVertSegVec.back().first);
    IntervalTree itvTree(1, 60);
    std::vector<Segment> tmpSegVec; // for temporary result from interval tree
    Segment tmpSeg;
    for(size_t idx = 0; idx < rectVertSegVec.size(); ++idx)
    {       
        tmpSegVec.clear();
        // encounter left vertical edge of one rectangle
        if( rectVertSegVec[idx].first == rectVec[rectVertSegVec[idx].second].lowerLeft_.x_ )
        {   
            itvTree.reverseOverlap(rectVec[rectVertSegVec[idx].second].lowerLeft_.y_, 
                                   rectVec[rectVertSegVec[idx].second].upperRight_.y_,
                                   tmpSegVec          
                                  );
                                    
            itvTree.insert(rectVec[rectVertSegVec[idx].second].lowerLeft_.y_, 
                           rectVec[rectVertSegVec[idx].second].upperRight_.y_);
                                    
            for(size_t idy = 0; idy < tmpSegVec.size(); ++idy)
            {   
            
                //set correct direction: downward                           
                tmpSeg.head_.y_ = std::max(tmpSegVec[idy].head_.y_,tmpSegVec[idy].tail_.y_);
                tmpSeg.tail_.y_ = std::min(tmpSegVec[idy].head_.y_,tmpSegVec[idy].tail_.y_);
                //set correct x coordinate
                tmpSeg.head_.x_ = rectVec[rectVertSegVec[idx].second].lowerLeft_.x_;
                tmpSeg.tail_.x_ = rectVec[rectVertSegVec[idx].second].lowerLeft_.x_;
                vertSegVec.push_back(tmpSeg);
            }
        }
        else // encounter right vertical edge of one rectangle
        {   
            itvTree.remove(rectVec[rectVertSegVec[idx].second].lowerLeft_.y_, 
                           rectVec[rectVertSegVec[idx].second].upperRight_.y_
                           );
            
            itvTree.reverseOverlap(rectVec[rectVertSegVec[idx].second].lowerLeft_.y_, 
                                   rectVec[rectVertSegVec[idx].second].upperRight_.y_,
                                   tmpSegVec          
                                  );
                
            for(size_t idy = 0; idy < tmpSegVec.size(); ++idy)
            {   
                //set correct direction: upward         
                tmpSeg.head_.y_ = std::min(tmpSegVec[idy].head_.y_,tmpSegVec[idy].tail_.y_);
                tmpSeg.tail_.y_ = std::max(tmpSegVec[idy].head_.y_,tmpSegVec[idy].tail_.y_);
                //set correct x coordinate
                tmpSeg.head_.x_ = rectVec[rectVertSegVec[idx].second].upperRight_.x_;
                tmpSeg.tail_.x_ = rectVec[rectVertSegVec[idx].second].upperRight_.x_;
                vertSegVec.push_back(tmpSeg);
            }
        }                              

    }
    for(size_t idx = 0; idx < vertSegVec.size(); ++idx)
    {
        printf("(%d,%d) (%d,%d)\n",vertSegVec[idx].head_.x_, vertSegVec[idx].head_.y_,
                                   vertSegVec[idx].tail_.x_, vertSegVec[idx].tail_.y_ );
    }
}
bool sortByYX(const std::pair<Point,size_t>& a, const std::pair<Point,size_t>& b)
{
    bool rtv = (a.first.y_ != b.first.y_) ? (a.first.y_ < b.first.y_) : (a.first.x_ < b.first.x_) ;
    return rtv;
    
}
class SegmentLink
{
    public:
        //SegmentLink() :head_(0),tail_(0) { seg_.head_ };
        SegmentLink(const Segment& seg, size_t headId, size_t tailId):
            seg_(seg),headId_(headId),tailId_(tailId) {};
            
        //SegmentLink transform(const Segment& input) const
        //{ return SegmentLink(input,0,0); };
            
        Segment seg_;
        size_t  headId_; //segment on the left or at bottom
        size_t  tailId_; //segment on the right or at top
};

void formContourByVertSeg(const std::vector<Segment>& vertSegVec, std::vector<Segment>& contourSegVec)
{
    //===============================================================
    // Will not check the validity of the input data because it is
    // too time consuming. Make sure the input is valid yourself.
    //================================================================
    
    if( vertSegVec.size() < 2 ) return;
    
    //triplet content: Point/which vertical seg
    Point tmpPt = {.x_=0,.y_=0};
    std::vector<std::pair<Point,size_t> > tripleVec;
    tripleVec.reserve(vertSegVec.size()*2);  
    for (size_t idx = 0; idx < vertSegVec.size(); ++idx)
    {
        tmpPt = vertSegVec[idx].head_;
        tripleVec.push_back( std::pair<Point,size_t>(tmpPt,idx) ) ;
        tmpPt = vertSegVec[idx].tail_;
        tripleVec.push_back( std::pair<Point,size_t>(tmpPt,idx) ) ;
    }
    
    // sort by y first and then by x. this is for forming horizontal segements
    std::sort(tripleVec.begin(), tripleVec.end(), sortByYX);
    
    //Basic idea is to build an array that contains the information of either vertical or horizontal segment and
    //with the information of its next and previous segments in the contour.
    // DETAIL DESCRIPTION:
    //   The sinlge element in the array contains:
    //   headId_ == Segment(vertical/horiztonal) == tailId_
    //
    // EXPLANATION:
    //   headId_ : the index in this array. 
    //             it means the left vertical segment of a horizontal segment if the segment of the element is horizontal.               
    //             it means the bottom horizontal segment of a vertical segment if the segment of the element is veritcal.  
    //   tailId_ : the index in this array. 
    //             it means the right vertical segment of a horizontal segment if the segment of the element is horizontal.              
    //             it means the top horizontal segment of a vertical segment if the segment of the element is veritcal.
    //
    // The array is divided into two parts:
    // [ Vertical Segments ] [ Horizontal Segments ]
    // While building up the second part of this array, the procedure will establish the correct headId_ and tailId_ of
    // the vertical segments and horizontal segments   
    //reserve correct size in order to prevent it to realloc memory. 
    //
    std::vector<SegmentLink> segLinkVec;
    segLinkVec.reserve(2*vertSegVec.size());
    //transform the input vertical seg into new format          
    for(int idx =0; idx < vertSegVec.size(); ++idx)
    {
        segLinkVec.push_back(SegmentLink(vertSegVec[idx],0,0));
    }
    
    //put in the horizontal segments
    Segment tmpSeg;
    for(size_t idx = 1; idx <= tripleVec.size()-1; idx+=2 )
    {
/*      assert((tripleVec[idx].first.y_ == vertSegVec[tripleVec[idx].second].head_.y_ && 
                tripleVec[idx].first.y_ == vertSegVec[tripleVec[idx-1].second].tail_.y_ ) ||
               (tripleVec[idx].first.y_ == vertSegVec[tripleVec[idx].second].tail_.y_ && 
                tripleVec[idx].first.y_ == vertSegVec[tripleVec[idx-1].second].head_.y_)
              );*/
        if(tripleVec[idx].first.y_ == vertSegVec[tripleVec[idx].second].tail_.y_)
        {
            //right to left
            tmpSeg.head_ = tripleVec[idx].first; 
            tmpSeg.tail_ = tripleVec[idx-1].first; 
            segLinkVec.push_back(SegmentLink(tmpSeg, tripleVec[idx].second, tripleVec[idx-1].second));
            segLinkVec[tripleVec[idx-1].second].headId_ = segLinkVec.size()-1;
            segLinkVec[tripleVec[idx].second].tailId_   = segLinkVec.size()-1;
        }
        else
        {
            //left to right
            tmpSeg.head_ = tripleVec[idx-1].first;
            tmpSeg.tail_ = tripleVec[idx].first; 
            segLinkVec.push_back(SegmentLink(tmpSeg, tripleVec[idx-1].second, tripleVec[idx].second));
            segLinkVec[tripleVec[idx].second].headId_ = segLinkVec.size()-1;
            segLinkVec[tripleVec[idx-1].second].tailId_ = segLinkVec.size()-1;
        }           
    }

    for(size_t idx = 0; idx < segLinkVec.size(); ++idx )
    {
        printf("segmentLink:[%u] (%d,%d)-(%d,%d) head:%u tail:%u\n", idx,
                segLinkVec[idx].seg_.head_.x_,segLinkVec[idx].seg_.head_.y_,
                segLinkVec[idx].seg_.tail_.x_,segLinkVec[idx].seg_.tail_.y_,
                segLinkVec[idx].headId_, segLinkVec[idx].tailId_
              );
    }       
    contourSegVec.reserve(2*vertSegVec.size());
    for(size_t idx = 0; idx < segLinkVec.size(); ++idx )
    {
        if ( segLinkVec[idx].headId_ ==  segLinkVec[idx].tailId_ ) continue;
        
        size_t startIdx = idx;
        size_t currIdx  = startIdx;
        size_t prevIdx  = currIdx;
        do
        {
            contourSegVec.push_back(segLinkVec[currIdx].seg_);  
            prevIdx  = currIdx;
            currIdx = segLinkVec[currIdx].tailId_;
            segLinkVec[prevIdx].headId_ = 0;
            segLinkVec[prevIdx].tailId_ = 0;
        } while(startIdx != currIdx);
    }

    for (size_t idx = 0; idx < contourSegVec.size(); ++idx )
    {
        printf("CONTOUR: (%d,%d)-(%d,%d)\n", contourSegVec[idx].head_.x_,contourSegVec[idx].head_.y_,
                                             contourSegVec[idx].tail_.x_,contourSegVec[idx].tail_.y_
              );
    }
}
                        
class AA
{
    public:
        AA() {printf("A::ctor\n"); kk = 1;}
       ~AA() {printf("A::dtor\n");}
        AA(const AA& rhs) {printf("A::cpyTor\n");};
       
    private:
        int kk;   
};
#include<bits/stl_tree.h>
int main(int argc, char* argv[])
{
#if 0
    IntervalTree itvTree2(1, 60);
    itvTree2.insert(1,10);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.insert(51,60);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.insert(2,56);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.insert(3,5);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.insert(53,55);
    itvTree2.print(),printf("=============================\n");;
    
    itvTree2.remove(2,56);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.remove(1,10);
    itvTree2.print(),printf("=============================\n");;
    itvTree2.remove(51,60);
    itvTree2.print(),printf("=============================\n");;
    std::vector<Segment> segVec2;
    itvTree2.reverseOverlap(1,10,segVec2);
    printf("======= reverse overlap ========\n");
    for(int idx = 0; idx < segVec2.size(); ++idx)
    {
        printf("(%d,%d) \n",segVec2[idx].head_.y_,segVec2[idx].tail_.y_);
    }   
    itvTree2.remove(51,60);
    itvTree2.print(),printf("=============================\n");;
    
    return 0;   
#endif    
    // you should test the case,:
    // itvTree(0, 1000);
    // itvTree.insert(0,500);
    // itvTree.remove(711,800); 
    // the result should do nothing
    IntervalTree itvTree(0, 1000);
    itvTree.insert(0,500);
    //itvTree.insert(711,800);
    itvTree.print();
    ItvTreeUtil::checkInsert(itvTree.getRoot());
    printf("Delete\n");
    itvTree.remove(711,800); 
    itvTree.print();
    ItvTreeUtil::checkInsert(itvTree.getRoot());
    printf("===>%p \n", itvTree.getRoot()); 
    
//=====================================================//
    printf("====================Test Reverse Overlap====================\n");
    itvTree.clear();
    itvTree.insert(0,1000);
    //itvTree.print();
    //itvTree.remove(0,500); 
    itvTree.remove(260,750); 
    itvTree.print();
    ItvTreeUtil::checkInsert(itvTree.getRoot());
    std::vector<Segment> segVec;
    itvTree.reverseOverlap(0,1000,segVec);
    printf("======= reverse overlap ========\n");
    for(int idx = 0; idx < segVec.size(); ++idx)
    {
        printf("(%d,%d) \n",segVec[idx].head_.y_,segVec[idx].tail_.y_);
    }   
    printf("====================End of Test Reverse Overlap====================\n");
    //         --------
    //         |      | 
    //  ------------------------
    //  |                      |
    //  |                      |
    //  ------------------------
    //         |      |
    //         --------
    std::vector<Rect>    rectVec;
    std::vector<Segment> verticalSegVec;
    std::vector<Segment> contourSegVec;
    rectVec.push_back(Rect{ Point{1,1} , Point{5,30} } );
    rectVec.push_back(Rect{ Point{0,10} , Point{30,15} } );
    getVerticalSeg(rectVec, verticalSegVec);    
    formContourByVertSeg(verticalSegVec, contourSegVec);
    printf("====================End of testing merging rectangles ===============\n");
    
    printf("==============Start of Testing harder shapes===============\n");
    rectVec.clear();
    verticalSegVec.clear();
    contourSegVec.clear();
    Rect tmpRect = { {1,1}, {10,10} }; 
    rectVec.push_back(tmpRect);
    
    tmpRect.lowerLeft_.x_ += 30; tmpRect.upperRight_.x_ += 30;
    rectVec.push_back(tmpRect);
    
    tmpRect.lowerLeft_.y_ += 50; tmpRect.upperRight_.y_ += 50;
    rectVec.push_back(tmpRect);
    
    tmpRect.lowerLeft_.x_ -= 30; tmpRect.upperRight_.x_ -= 30;
    rectVec.push_back(tmpRect);
    
    //horizontal strip
    tmpRect.lowerLeft_.x_  = 3;  tmpRect.lowerLeft_.y_  = 3;
    tmpRect.upperRight_.x_ = 35; tmpRect.upperRight_.y_ = 5;
    rectVec.push_back(tmpRect);
    tmpRect.lowerLeft_.y_  += 50; tmpRect.upperRight_.y_ += 50;
    rectVec.push_back(tmpRect);
    
    //vertical strip
    tmpRect.lowerLeft_.x_  = 2;  tmpRect.lowerLeft_.y_  = 2;
    tmpRect.upperRight_.x_ = 4;  tmpRect.upperRight_.y_ = 56;
    rectVec.push_back(tmpRect);
    tmpRect.lowerLeft_.x_  += 30; tmpRect.upperRight_.x_ += 30;
    rectVec.push_back(tmpRect);
    
    getVerticalSeg(rectVec, verticalSegVec);    
    formContourByVertSeg(verticalSegVec, contourSegVec);
    printf("==============END of Testing harder shapes===============\n");
 
    std::_Identity<int> a;
    std::_Rb_tree<int,int,std::_Identity<int>, std::less<int> > aa;
 

    return EXIT_SUCCESS;
}
