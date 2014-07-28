#ifndef _GEOOBJ_H_
#define _GEOOBJ_H_

#ifndef _BASETYPES_H_
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
