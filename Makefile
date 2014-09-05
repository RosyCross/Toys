
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
CPPSUFFIX = cpp

#=====================
#    Build Library 
#=====================
intervalTree.o : geoObj.h baseTypes.h intervalTree.h intervalTree.cpp
	${CXX} ${STDARGS} ${CXXFLAGS} -c intervalTree.cpp -o intervalTree.o

#=====================
#    Build executable 
#=====================
testGraphLib.${EXESUFFIX} : graphLib.hpp testGraphLib.cpp 
	${CXX} ${STDARGS} ${CXXFLAGS} ${@:.${EXESUFFIX}=.${CPPSUFFIX}} -o $@ 

testIntervalTree.${EXESUFFIX} : intervalTree.o testIntervalTree.cpp
	${CXX} ${STDARGS} ${CXXFLAGS} ${@:.${EXESUFFIX}=.${CPPSUFFIX}} intervalTree.o -o $@ 
	
testLongestPath.${EXESUFFIX} : graphLib.hpp graphUtil.hpp testLongestPath.cpp
	${CXX} ${STDARGS} ${CXXFLAGS} ${@:.${EXESUFFIX}=.${CPPSUFFIX}} -o $@

#
# For auto generate depend file
#
include testGraphLib.d

%.d : %.cpp
	echo "start to generate depend file";\
	${CXX} -M ${CXXFLAGS} $< > $@.$$$$;\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	#rm -f $@.$$$$
