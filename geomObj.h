#ifndef GEOMOBJ_H_
#define GEOMOBJ_H_

#ifndef BASETYPES_H_
#include "baseTypes.h"
#endif

//=========================================
// Integral Geometry Primitive Objects
//=========================================
struct Point
{
    IntType x_;
    IntType y_;
};

struct Segment
{
    Point head_;
    Point tail_;
};

struct Rect
{
    Point lowerLeft_;
    Point upperRight_;
};

#endif
