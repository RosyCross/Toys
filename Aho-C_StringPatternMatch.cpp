#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <set>


struct {
       char a; 
       double b; 
       char c;
} INFO_S[5] = {
   {'a',3,'b'} ,          
};

const char* gc_apcKeyword[] =
{
   "he",
   "she",
   "his",
   "hers"   
};
const int g_nKeywordSize = sizeof(gc_apcKeyword)/sizeof(gc_apcKeyword[0]);

typedef int StateId;
typedef char InputObj;
class State_C
{
     public:
            State_C(): m_oStateId(-1) {};
            StateId m_oStateId;
            std::map<InputObj,StateId> m_oOutGoing;
            std::vector<std::string>   m_oOutput;
};

void ConstructFiniteMachine(int p_nKeywordSize, const char* p_apcKeyword[], std::vector<State_C>& p_roStateFM)
{
     if ( 0 == p_nKeywordSize || NULL == p_apcKeyword ) return;
     
     //Set START STATE first   
     p_roStateFM.clear();
     p_roStateFM.push_back( State_C() );
     p_roStateFM[0].m_oStateId = 0;
     
     std::map<InputObj,StateId>::iterator itor;
     int nCurrStateIdx = 0; //Always set out from START STATE
     int nStrLen = -1;
     for (int nIdx = 0; nIdx < p_nKeywordSize; ++nIdx)
     {
         if ( NULL == p_apcKeyword[nIdx] || '\0' == *(p_apcKeyword[nIdx]) )
             continue;
             
         //Make abbreviatione only
         //State_C& oCurrState = p_roStateFM[nCurrStateIdx];
         nCurrStateIdx = 0;
         nStrLen = strlen( p_apcKeyword[nIdx] );
         int nPtrIdx = 0;
         while ( nPtrIdx < nStrLen &&
                 p_roStateFM[nCurrStateIdx].m_oOutGoing.end() != 
                 (itor = p_roStateFM[nCurrStateIdx].m_oOutGoing.find(p_apcKeyword[nIdx][nPtrIdx])) )
         {
             printf("%d-", nCurrStateIdx);    
             nCurrStateIdx = (*itor).second;
             ++nPtrIdx;                  
         }        
         assert(nPtrIdx < nStrLen);
         printf("%d-", nCurrStateIdx);
         
         while ( nPtrIdx < nStrLen )
         {
             p_roStateFM.push_back(State_C());
             p_roStateFM.back().m_oStateId = p_roStateFM.size() - 1;
             p_roStateFM[nCurrStateIdx].m_oOutGoing[ p_apcKeyword[nIdx][nPtrIdx] ] = p_roStateFM.size() - 1;
             nCurrStateIdx = p_roStateFM.size() - 1;           
             ++nPtrIdx;
             printf("%d=", nCurrStateIdx);
         }

         p_roStateFM[nCurrStateIdx].m_oOutput.push_back( p_apcKeyword[nIdx] );
         printf("Push In:%s at %d\n",p_apcKeyword[nIdx], nCurrStateIdx);        
     }//----- for -----
}

void ConstructFailFunction(std::vector<State_C>& p_roStateFM, std::map<StateId,StateId>& p_roFailFunc)
{
     std::queue<StateId> oQueue;

     //depth 1 fail function     
     printf("BFS ");
     std::map<InputObj,StateId>::const_iterator itor = p_roStateFM[0].m_oOutGoing.begin(); 
     while ( p_roStateFM[0].m_oOutGoing.end() != itor )
     {
           printf("%d-",(*itor).second);
           oQueue.push( (*itor).second );
           p_roFailFunc[ (*itor).second ] = 0;
           ++itor;
     }
     printf("\n");
     
     //depth larger than 1 fail function 
     StateId oCurrStateId = -1;
     StateId oTmpStateId = -1;
     std::map<InputObj,StateId>::iterator tmpIter;
     while ( 0 < oQueue.size() )
     {
         oCurrStateId = oQueue.front();      
         oQueue.pop();
         
         itor = p_roStateFM[oCurrStateId].m_oOutGoing.begin();
         while ( p_roStateFM[oCurrStateId].m_oOutGoing.end() != itor )
         {
  
             oQueue.push( (*itor).second );
             if ( p_roFailFunc.end() == p_roFailFunc.find( oCurrStateId ) )
                 assert(false);
             
             //oTmpStateId = (*itor).second;
             oTmpStateId = p_roFailFunc[oCurrStateId];
             printf("First Fail Trans: %d <- %d \n", oTmpStateId,oCurrStateId);
             while ( p_roStateFM[oTmpStateId].m_oOutGoing.end() == 
                     (tmpIter = p_roStateFM[oTmpStateId].m_oOutGoing.find( (*itor).first ))
                      &&
                     0 < oTmpStateId 
                   )
             {          
                 oTmpStateId = p_roFailFunc[oTmpStateId];
             }    
                 
             if (  p_roStateFM[oTmpStateId].m_oOutGoing.end() != tmpIter )
                 p_roFailFunc[ (*itor).second ] =  (*tmpIter).second, printf("%d <--- %d\n",(*itor).second, (*tmpIter).second);
             else
                 p_roFailFunc[ (*itor).second ] =  0, printf("%d <--- %d\n",(*itor).second, 0);

             if ( 0 < oTmpStateId && p_roStateFM[oTmpStateId].m_oOutGoing.end() != tmpIter )
             {
             p_roStateFM[(*itor).second].m_oOutput.insert(  
             p_roStateFM[(*itor).second].m_oOutput.end(),
             p_roStateFM[(*tmpIter).second].m_oOutput.begin(), 
             p_roStateFM[(*tmpIter).second].m_oOutput.end()
             );
             }
             
             //printf("Merge %d to %d kkk\n", oTmpStateId,(*tmpIter).second);                        
             for (int nIdx = 0; nIdx < p_roStateFM[oTmpStateId].m_oOutput.size(); ++nIdx )             
             {
                 printf("Merge %d to %d -- %s\n", oTmpStateId,(*itor).second, p_roStateFM[oTmpStateId].m_oOutput[nIdx].c_str() );
             }
             ++itor;
         }//----- while -----
     }//----- while -----
}

void Aho_Corasick_Pattern_Match(
const char *p_pcTextString,
const std::vector<State_C>& p_roStateFM, 
const std::map<StateId,StateId>& p_roFailFunc )
{
    if ( NULL == p_pcTextString || '\0' == *p_pcTextString ) return;     
    
    //Start State
    StateId oStateId = 0;
    std::map<InputObj,StateId>::const_iterator itor = 
    p_roStateFM[oStateId].m_oOutGoing.find(*p_pcTextString);
    while ( '\0' != *p_pcTextString )
    {
          //Make a abbreviation only
          //const std::map<InputObj,StateId>& oTransMap = p_roStateFM[oStateId].m_oOutGoing;
          while ( p_roStateFM[oStateId].m_oOutGoing.end() == 
                 (itor = p_roStateFM[oStateId].m_oOutGoing.find(*p_pcTextString)) 
                  &&
                  oStateId > 0
                 )
          {
              oStateId = static_cast<std::map<StateId,StateId> >(p_roFailFunc)[oStateId];      
          }
          //Make State Transistion
          if ( p_roStateFM[oStateId].m_oOutGoing.end() != itor )             
              oStateId = (*itor).second;
          else
              oStateId = 0;    
          
          //Output Matching
          if ( 0 < p_roStateFM[oStateId].m_oOutput.size() )
          {
              for (int nIdx = 0; nIdx < p_roStateFM[oStateId].m_oOutput.size(); ++nIdx)     
              {
                  printf("%c -- %s\n", *p_pcTextString, p_roStateFM[oStateId].m_oOutput[nIdx].c_str());    
              }
          }
          
          //move to next char
          ++p_pcTextString;
    }
}

class Status_C
{
      public:
         //std::vector<int> GroupAry;    
         std::vector<std::map<StateId, std::vector<StateId> >::iterator > GroupAry;    
};

void ConstructNextFunction(
const std::vector<State_C>& p_roStateFM, 
const std::map<StateId,StateId>& p_roFailFunc,
std::map<StateId,StateId> &p_roResult,
std::vector< std::map<InputObj,StateId> > &p_roResultMapVect
)
{
     //For BFS 
     std::queue<StateId> oQueue;

     //For intermediate data structure
     // current -- hash of ( char , Next)
     std::map<StateId, std::map<InputObj,StateId> > oFatNextFunc;

     //For intermediate data structure
     // char -- hash of ( Next, From )
     //std::map<InputObj, std::map<StateId,std::vector<StateId> > > oKeeper;
     std::map<InputObj, std::map<StateId,std::set<StateId> > > oKeeper;
     
     //depth 1 Next function          
     std::map<InputObj,StateId>::const_iterator itor = p_roStateFM[0].m_oOutGoing.begin(); 
     while ( p_roStateFM[0].m_oOutGoing.end() != itor )
     {
         oQueue.push( (*itor).second );
         oFatNextFunc[0][ (*itor).first ] = (*itor).second;
         oKeeper[ (*itor).first ][ (*itor).second ].insert( 0 );
         ++itor;      
     }
     
     //depth 2 and beyond Next function   
     StateId oCurrStateId = -1;
     StateId oFailStateId = -1; //from fail function  
     std::map<InputObj,StateId>::const_iterator oNextStateIter; 
     std::map<InputObj,StateId>::const_iterator oTmpIter; 
     while ( 0 < oQueue.size() )
     {
         oCurrStateId = oQueue.front();
         oQueue.pop();
         
         //iterate throught the outgoing States that are from current state
         itor = p_roStateFM[oCurrStateId].m_oOutGoing.begin();
         while ( p_roStateFM[oCurrStateId].m_oOutGoing.end() != 
                 itor
                )
         {
             //For BFS
             printf("Fat Next: %d - %c - %d\n",oCurrStateId,(*itor).first, (*itor).second);
             oQueue.push( (*itor).second );
             oFatNextFunc[ oCurrStateId ][ (*itor).first ] = (*itor).second;               
             oKeeper[ (*itor).first ][ (*itor).second ].insert(oCurrStateId);                                               
             ++itor;       
         } // ----- while -----        
         
         oFailStateId = static_cast<std::map<StateId,StateId> >(p_roFailFunc)[oCurrStateId];    
         oTmpIter = oFatNextFunc[oFailStateId].begin();
         while ( oTmpIter != oFatNextFunc[oFailStateId].end() )
         {             
             if (
                 p_roStateFM[oCurrStateId].m_oOutGoing.end() == 
                 p_roStateFM[oCurrStateId].m_oOutGoing.find( (*oTmpIter).first ) 
                )
             {   
             printf("kkk Next: %d - %c - %d = %d\n",oCurrStateId,(*oTmpIter).first, (*oTmpIter).second,
                     p_roStateFM[oCurrStateId].m_oOutGoing.end() == 
                 p_roStateFM[oCurrStateId].m_oOutGoing.find( (*oTmpIter).first )  );          
                 
             oFatNextFunc[oCurrStateId][ (*oTmpIter).first ] = (*oTmpIter).second;
             oKeeper[ (*oTmpIter).first ][ (*oTmpIter).second ].insert(oCurrStateId);
             }
             ++oTmpIter;  
         }        
     }//----- while -----       
     
     
     //build graph     
     std::map< StateId, std::map< StateId, int > > oStateRelation;
     //std::map<StateId,StateId> oResult;
     //std::vector< std::map<InputObj,StateId> > p_roResultMapVect;
     p_roResultMapVect.push_back( std::map<InputObj,StateId>() );//dummy
     
     std::map<StateId, std::map<InputObj,StateId> >::iterator
     oGrpItor = oFatNextFunc.begin();

     std::map<InputObj, StateId>::iterator oStIt;

     std::set<StateId> oCheckSet;
     std::set<StateId> oDoneSet;
     
     std::vector<StateId> oTmpVect;
     oCurrStateId = -1;       
     while ( oFatNextFunc.end() != oGrpItor  )
     {         
         if ( oDoneSet.find( (*oGrpItor).first ) != oDoneSet.end() )                       
         {
              ++oGrpItor;
              continue;
         }
         assert( 0 < (*oGrpItor).second.size() );
         for ( oStIt = (*oGrpItor).second.begin();  
               oStIt != (*oGrpItor).second.end() ;
               ++oStIt
              )
         {
           if ( oCheckSet.size() <= 0)
           {
               for (std::set<StateId>::iterator oSetIt = oKeeper[ (*oStIt).first ][ (*oStIt).second ].begin() ; 
                    oSetIt !=  oKeeper[ (*oStIt).first ][ (*oStIt).second ].end(); 
                    ++oSetIt )
                   oCheckSet.insert( (*oSetIt) ); 
                   
               continue;    
           }   
           
           std::vector<StateId>().swap(oTmpVect);
           for (  std::set<StateId>::iterator oSetIt = oCheckSet.begin() ; 
                  oSetIt !=  oCheckSet.end(); ++oSetIt )
           {
               if ( oKeeper[ (*oStIt).first ][ (*oStIt).second ].end() ==  
                    oKeeper[ (*oStIt).first ][ (*oStIt).second ].find( (*oSetIt) ) 
                    ||
                    oFatNextFunc[(*oSetIt)].size() != (*oGrpItor).second.size()
                  )
               {
                    oTmpVect.push_back( (*oSetIt) );
               }          
               //printf("relation: %c %d - %d\n", (*oGrpItor).first, (*oStIt)., (*oStIt) );        
           } //---- for ----                
           for ( int nIdx = 0; nIdx < oTmpVect.size(); ++nIdx )
           {
               oCheckSet.erase( oTmpVect[nIdx] );
           }
           
         }// ---- for ----         
         oCheckSet.insert( (*oGrpItor).first );  
         printf("Same Group:");
         for (  std::set<StateId>::iterator oSetIt = oCheckSet.begin() ; 
                  oSetIt !=  oCheckSet.end(); ++oSetIt )
         {
             printf(" %d ", (*oSetIt) );        
             oDoneSet.insert( (*oSetIt) );
         }
         oCurrStateId = *( oCheckSet.begin() );
         p_roResultMapVect.push_back( std::map<InputObj,StateId>( oFatNextFunc[oCurrStateId] ) );         
         p_roResult[oCurrStateId] = -(p_roResultMapVect.size()-1);  
         for (  std::set<StateId>::iterator oSetIt = ++oCheckSet.begin(); 
                oSetIt !=  oCheckSet.end(); ++oSetIt )
         {
             p_roResult[(*oSetIt)] = -(p_roResultMapVect.size()-1);;  
         }                  
         printf("\n");
         oCheckSet.clear();
         ++oGrpItor;      
     }
     printf("group count %d %d \n", p_roResultMapVect.size()-1);
     system("PAUSE");     
}

void Fast_Aho_Corasick_Pattern_Match(
const char *p_pcTextString,
const std::vector<State_C>& p_roStateFM, 
std::map<StateId,StateId> &p_roResult,
std::vector< std::map<InputObj,StateId> > &p_roResultMapVect
)
{
    if ( NULL == p_pcTextString || '\0' == *p_pcTextString )
        return;

    StateId oCurrStateId = 0;
    while ( '\0' != *p_pcTextString )
    {
        if ( p_roResultMapVect[ (-p_roResult[oCurrStateId]) ].end() 
             != p_roResultMapVect[ (-p_roResult[oCurrStateId]) ].find(*p_pcTextString) )
        {

            oCurrStateId = p_roResultMapVect[ (-p_roResult[oCurrStateId]) ][ *p_pcTextString ] ;
        }
        else
        {
            oCurrStateId = 0;
        }                      

        if ( 0 < p_roStateFM[oCurrStateId].m_oOutput.size() )
        {
            printf("Fast- ");
            for (int nIdx = 0; nIdx < p_roStateFM[oCurrStateId].m_oOutput.size(); ++nIdx)
            {
                printf("%s ",p_roStateFM[oCurrStateId].m_oOutput[nIdx].c_str() );
            }
            printf("\n");
        }

        ++p_pcTextString;
    }
}
 
int main(int argc, char** argv)
{
    
    std::vector<State_C> oStateFM;
    std::map<StateId,StateId> oFailFunc;

    ConstructFiniteMachine(g_nKeywordSize,gc_apcKeyword,oStateFM);    
    ConstructFailFunction(oStateFM, oFailFunc);
    Aho_Corasick_Pattern_Match("usher", oStateFM, oFailFunc );

    std::map<StateId,StateId> oResult;
    std::vector< std::map<InputObj,StateId> > oResultMapVect;
    ConstructNextFunction(oStateFM, oFailFunc, oResult, oResultMapVect);
Fast_Aho_Corasick_Pattern_Match(
"usher",
oStateFM, 
oResult,
oResultMapVect
);

    system("PAUSE");    
    return EXIT_SUCCESS;
}
