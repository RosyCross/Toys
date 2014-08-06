
#================
#     MACRO
#================
CXX = clang++
#for setting up desirable version of compiler with correct STL library
STDARGS = -std=c++11 -stdlib=libstdc++
CXXFLAGS =
CXXLIBS =

#add suffix for user to rapidly locate the executable
EXESUFFIX = exe

#=====================
#    Build Library 
#=====================
intervalTree.o : geoObj.h baseTypes.h intervalTree.h intervalTree.cpp
	${CXX} ${STDARGS} ${CXXFLAGS} -c intervalTree.cpp -o intervalTree.o

#=====================
#    Build executable 
#=====================
testGraphLib : GraphLib.hpp testGraphLib.cpp 
	${CXX} ${STDARGS} ${CXXFLAGS} testGraphLib.cpp -o testGraphLib.exe

testIntervalTree.${EXESUFFIX} : intervalTree.o
	${CXX} ${STDARGS} ${CXXFLAGS} testIntervalTree.cpp intervalTree.o -o testIntervalTree.${EXESUFFIX} 
	
