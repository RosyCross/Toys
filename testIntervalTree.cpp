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
#include "geomObj.h"

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
        rectVertSegVec.push_back(std::pair<IntType,size_t>(rectVec[idx].getLL().getX(), idx) );
        rectVertSegVec.push_back(std::pair<IntType,size_t>(rectVec[idx].getUR().getX(), idx) );
    }   
    assert( 0 == (rectVertSegVec.size() & 1) );

    //=====================================     
    //pre-sort by x for Scan line method; 
    //=====================================
    std::sort(rectVertSegVec.begin(),rectVertSegVec.end(),cmpRectVertSeg);
    for(size_t idx = 0; idx< rectVertSegVec.size(); ++idx)
    {
        printf("x:%d --- (%d, %d)=(%d,%d)\n", rectVertSegVec[idx].first,
                rectVec[rectVertSegVec[idx].second].getLL().getX(),
                rectVec[rectVertSegVec[idx].second].getLL().getY(),
                rectVec[rectVertSegVec[idx].second].getUR().getX(),
                rectVec[rectVertSegVec[idx].second].getUR().getY()
              );
    }
    
    //=================================================================
    //get the vertical segments of the contour of the merged rectangles;
    //=================================================================
    //IntervalTree itvTree(rectVertSegVec.front().first, rectVertSegVec.back().first);
    IntervalTree itvTree(1, 60);
    std::vector<Segment> tmpSegVec; // for temporary result from interval tree
    Segment tmpSeg(Point(0,0),Point(0,0));
    for(size_t idx = 0; idx < rectVertSegVec.size(); ++idx)
    {       
        tmpSegVec.clear();
        // encounter left vertical edge of one rectangle
        if( rectVertSegVec[idx].first == rectVec[rectVertSegVec[idx].second].getLL().getX() )
        {   
            itvTree.reverseOverlap(rectVec[rectVertSegVec[idx].second].getLL().getY(), 
                                   rectVec[rectVertSegVec[idx].second].getUR().getY(),
                                   tmpSegVec          
                                  );
                                    
            itvTree.insert(rectVec[rectVertSegVec[idx].second].getLL().getY(), 
                           rectVec[rectVertSegVec[idx].second].getUR().getY());
                                    
            for(size_t idy = 0; idy < tmpSegVec.size(); ++idy)
            {   
            
                //set correct direction: downward                           
                //tmpSeg.getHead().getY() = std::max(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY());
                tmpSeg.setHeadY( std::max(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY()) );
                //tmpSeg.getTail().getY() = std::min(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY());
                tmpSeg.setTailY( std::min(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY()) );

                //set correct x coordinate
                //tmpSeg.getHead().getX() = rectVec[rectVertSegVec[idx].second].getLL().getX();
                tmpSeg.setHeadX( rectVec[rectVertSegVec[idx].second].getLL().getX() );

                //tmpSeg.getTail().getX() = rectVec[rectVertSegVec[idx].second].getLL().getX();
                tmpSeg.setTailX( rectVec[rectVertSegVec[idx].second].getLL().getX() );

                vertSegVec.push_back(tmpSeg);
            }
        }
        else // encounter right vertical edge of one rectangle
        {   
            itvTree.remove(rectVec[rectVertSegVec[idx].second].getLL().getY(), 
                           rectVec[rectVertSegVec[idx].second].getUR().getY()
                           );
            
            itvTree.reverseOverlap(rectVec[rectVertSegVec[idx].second].getLL().getY(), 
                                   rectVec[rectVertSegVec[idx].second].getUR().getY(),
                                   tmpSegVec          
                                  );
                
            for(size_t idy = 0; idy < tmpSegVec.size(); ++idy)
            {   
                //set correct direction: upward         
                //tmpSeg.getHead().getY() = std::min(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY());
                tmpSeg.setHeadY( std::min(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY()) );
                //tmpSeg.getTail().getY() = std::max(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY());
                tmpSeg.setTailY( std::max(tmpSegVec[idy].getHead().getY(),tmpSegVec[idy].getTail().getY()) );

                //set correct x coordinate
                //tmpSeg.getHead().getX() = rectVec[rectVertSegVec[idx].second].getUR().getX();
                tmpSeg.setHeadX( rectVec[rectVertSegVec[idx].second].getUR().getX() );

                //tmpSeg.getTail().getX() = rectVec[rectVertSegVec[idx].second].getUR().getX();
                tmpSeg.setTailX( rectVec[rectVertSegVec[idx].second].getUR().getX() );

                vertSegVec.push_back(tmpSeg);
            }
        }                              

    }
    for(size_t idx = 0; idx < vertSegVec.size(); ++idx)
    {
        printf("(%d,%d) (%d,%d)\n",vertSegVec[idx].getHead().getX(), vertSegVec[idx].getHead().getY(),
                                   vertSegVec[idx].getTail().getX(), vertSegVec[idx].getTail().getY() );
    }
}
bool sortByYX(const std::pair<Point,size_t>& a, const std::pair<Point,size_t>& b)
{
    bool rtv = (a.first.getY() != b.first.getY()) ? (a.first.getY() < b.first.getY()) : (a.first.getX() < b.first.getX()) ;
    return rtv;
    
}
class SegmentLink
{
    public:
        //SegmentLink() .getHead()(0).getTail()(0) { seg_.getHead() };
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
    Point tmpPt(0,0);
    std::vector<std::pair<Point,size_t> > tripleVec;
    tripleVec.reserve(vertSegVec.size()*2);  
    for (size_t idx = 0; idx < vertSegVec.size(); ++idx)
    {
        tmpPt = vertSegVec[idx].getHead();
        tripleVec.push_back( std::pair<Point,size_t>(tmpPt,idx) ) ;
        tmpPt = vertSegVec[idx].getTail();
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
    Segment tmpSeg(Point(0,0),Point(0,0));
    for(size_t idx = 1; idx <= tripleVec.size()-1; idx+=2 )
    {
/*      assert((tripleVec[idx].first.getY() == vertSegVec[tripleVec[idx].second].getHead().getY() && 
                tripleVec[idx].first.getY() == vertSegVec[tripleVec[idx-1].second].getTail().getY() ) ||
               (tripleVec[idx].first.getY() == vertSegVec[tripleVec[idx].second].getTail().getY() && 
                tripleVec[idx].first.getY() == vertSegVec[tripleVec[idx-1].second].getHead().getY())
              );*/
        if(tripleVec[idx].first.getY() == vertSegVec[tripleVec[idx].second].getTail().getY())
        {
            //right to left
            //tmpSeg.getHead() = tripleVec[idx].first; 
            tmpSeg.setHead( tripleVec[idx].first ); 
            //tmpSeg.getTail() = tripleVec[idx-1].first; 
            tmpSeg.setTail( tripleVec[idx-1].first );
            segLinkVec.push_back(SegmentLink(tmpSeg, tripleVec[idx].second, tripleVec[idx-1].second));
            segLinkVec[tripleVec[idx-1].second].headId_ = segLinkVec.size()-1;
            segLinkVec[tripleVec[idx].second].tailId_   = segLinkVec.size()-1;
        }
        else
        {
            //left to right
            //tmpSeg.getHead() = tripleVec[idx-1].first;
            tmpSeg.setHead( tripleVec[idx-1].first );
            //tmpSeg.getTail() = tripleVec[idx].first; 
            tmpSeg.setTail( tripleVec[idx].first ); 
            segLinkVec.push_back(SegmentLink(tmpSeg, tripleVec[idx-1].second, tripleVec[idx].second));
            segLinkVec[tripleVec[idx].second].headId_ = segLinkVec.size()-1;
            segLinkVec[tripleVec[idx-1].second].tailId_ = segLinkVec.size()-1;
        }           
    }

    for(size_t idx = 0; idx < segLinkVec.size(); ++idx )
    {
        printf("segmentLink:[%u] (%d,%d)-(%d,%d) head:%u tail:%u\n", idx,
                segLinkVec[idx].seg_.getHead().getX(),segLinkVec[idx].seg_.getHead().getY(),
                segLinkVec[idx].seg_.getTail().getX(),segLinkVec[idx].seg_.getTail().getY(),
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
        printf("CONTOUR: (%d,%d)-(%d,%d)\n", contourSegVec[idx].getHead().getX(),contourSegVec[idx].getHead().getY(),
                                             contourSegVec[idx].getTail().getX(),contourSegVec[idx].getTail().getY()
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
        printf("(%d,%d) \n",segVec2[idx].getHead().getY(),segVec2[idx].getTail().getY());
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
        printf("(%d,%d) \n",segVec[idx].getHead().getY(),segVec[idx].getTail().getY());
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
    
    //tmpRect.lowerLeft_.getX() += 30; tmpRect.upperRight_.getX() += 30;
    tmpRect.setLLX(tmpRect.getLLX()+30); tmpRect.setURX(tmpRect.getURX()+30);
    rectVec.push_back(tmpRect);
    
    //tmpRect.lowerLeft_.getY() += 50; tmpRect.upperRight_.getY() += 50;
    tmpRect.setLLY(tmpRect.getLLY()+50); tmpRect.setURY(tmpRect.getURY()+50);
    rectVec.push_back(tmpRect);
    
    //tmpRect.lowerLeft_.getX() -= 30; tmpRect.upperRight_.getX() -= 30;
    tmpRect.setLLX(tmpRect.getLLX()-30); tmpRect.setURX(tmpRect.getURX()-30);
    rectVec.push_back(tmpRect);
    
    //horizontal strip
    //tmpRect.lowerLeft_.getX()  = 3;  tmpRect.lowerLeft_.getY()  = 3;
    tmpRect.setLLX(3); tmpRect.setLLY(3);
    //tmpRect.upperRight_.getX() = 35; tmpRect.upperRight_.getY() = 5;
    tmpRect.setURX(35); tmpRect.setURY(5); 
    rectVec.push_back(tmpRect);
    //tmpRect.lowerLeft_.getY()  += 50; tmpRect.upperRight_.getY() += 50;
    tmpRect.setLLY(tmpRect.getLLY()+50); tmpRect.setURY(tmpRect.getURY()+50);
    rectVec.push_back(tmpRect);
    
    //vertical strip
    //tmpRect.lowerLeft_.getX()  = 2;  tmpRect.lowerLeft_.getY()  = 2;
    tmpRect.setLLX(2); tmpRect.setLLY(2);
    //tmpRect.upperRight_.getX() = 4;  tmpRect.upperRight_.getY() = 56;
    tmpRect.setURX(4); tmpRect.setURY(56);
    rectVec.push_back(tmpRect);
    //tmpRect.lowerLeft_.getX()  += 30; tmpRect.upperRight_.getX() += 30;
    tmpRect.setLLX(tmpRect.getLLX()+30); tmpRect.setURX(tmpRect.getURX()+30);
    rectVec.push_back(tmpRect);
    
    getVerticalSeg(rectVec, verticalSegVec);    
    formContourByVertSeg(verticalSegVec, contourSegVec);
    printf("==============END of Testing harder shapes===============\n");
 
//    std::_Identity<int> a;
//    std::_Rb_tree<int,int,std::_Identity<int>, std::less<int> > aa;
 

    return EXIT_SUCCESS;
}
