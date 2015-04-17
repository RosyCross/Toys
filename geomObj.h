#ifndef GEOMOBJ_H_
#define GEOMOBJ_H_

#ifndef BASETYPES_H_
#include "baseTypes.h"
#endif

//=========================================
// Integral Geometry Primitive Objects
//=========================================
/*
class Point
{
    public:
        Point(Coord x, Coord y):x_(x),y_(y){}
  const Coord&   getX() const { return x_; }
  const Coord&   getY() const { return y_; }
         void    setX(Coord x) { x_ = x; }
         void    setY(Coord y) { y_ = y; }


    private:
        Coord    x_;
        Coord    y_;
};
*/
template<typename T>
class _Point_
{
    public:
        _Point_(T x, T y):x_(x),y_(y){}
      const T&   getX() const { return x_; }
      const T&   getY() const { return y_; }
         void    setX(T x) { x_ = x; }
         void    setY(T y) { y_ = y; }


    private:
            T    x_;
            T    y_;
};
typedef _Point_<Coord> Point;

class Segment
{
    public:
        Segment(const Point& head, const Point& tail):head_(head),tail_(tail){}
  const Point&   getHead() const { return head_; }
  const Point&   getTail() const { return tail_; }
         void    setHead(const Point& head) { head_ = head; }
         void    setTail(const Point& tail) { tail_ = tail; }

         void    setHeadX(const Coord& newX) { head_.setX(newX); }
         void    setHeadY(const Coord& newY) { head_.setY(newY); }
         void    setTailX(const Coord& newX) { tail_.setX(newX); }
         void    setTailY(const Coord& newY) { tail_.setY(newY); }

    private:
        Point    head_;
        Point    tail_;
};

class Rect
{
    public:
        Rect(const Point& lowerLeft, const Point& upperRight):lowerLeft_(lowerLeft), upperRight_(upperRight){}

        const   Point& getLL() const { return lowerLeft_;  }
        const   Point& getUR() const { return upperRight_; }
         void   setLL(const Point& pt) { lowerLeft_ = pt; };
         void   setUR(const Point& pt) { upperRight_ = pt; };

         //set/get the x/y coordinate of the lowerLeft point
         void   setLLX(const Coord& newX) { lowerLeft_.setX(newX); }
         void   setLLY(const Coord& newY) { lowerLeft_.setY(newY); }
  const Coord&  getLLX() const { return lowerLeft_.getX(); }
  const Coord&  getLLY() const { return lowerLeft_.getY(); }


         //set/get the x/y coordinate of the upper right point
         void   setURX(const Coord& newX) { upperRight_.setX(newX); }
         void   setURY(const Coord& newY) { upperRight_.setY(newY); }
  const Coord&  getURX() const { return upperRight_.getX(); }
  const Coord&  getURY() const { return upperRight_.getY(); }

    private:
        Point   lowerLeft_;
        Point   upperRight_;
};

typedef Rect BBox; //they can use the same class

#endif
