#ifndef BASETYPES_H_
#define BASETYPES_H_

#ifndef _LIMITS_H___
    #include <limits.h>
#endif

typedef int IntType;
const IntType IntTypeMax = INT_MAX;
const IntType IntTypeMin = INT_MIN;

typedef int Coord;
const Coord COORD_MAX = INT_MAX;
const Coord COORD_MIN = INT_MIN;

#endif
