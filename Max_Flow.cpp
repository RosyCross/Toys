#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <set>

#define DEBUG

typedef int GraphNodeId ;
typedef int GraphNodeWeight;


struct NodeData_C
{
    //int m_nWeight;
    int m_nCapacity;
    int m_nFlow;       
};

std::vector< std::vector< GraphNodeId > >        g_oFlowNetwork;
std::map< GraphNodeId, std::map<GraphNodeId,int> >   g_oFlowNetwork2;
std::vector<NodeData_C> oDataDeposite; 

bool ReadData()
{
     GraphNodeId FromNode   = -1;
     GraphNodeId ToNode     = -1;
//     GraphNodeWeight Weight = -1;
     int nCapacity              = -1;
     NodeData_C oTmpNode = { m_nCapacity:-1, m_nFlow:-1};
     while ( 1 <= scanf("%d %d %d",&FromNode, &ToNode, &nCapacity ) )
     {
         printf("From:%d To:%d Flow:%d\n",FromNode, ToNode, nCapacity); 
         //printf("===\n");         
         oTmpNode.m_nCapacity = nCapacity;
         oTmpNode.m_nFlow     = 0;
         oDataDeposite.push_back(oTmpNode);
         g_oFlowNetwork2[FromNode][ToNode] = oDataDeposite.size() - 1;
     }
     
     return true;
}

int  BFS_AugmentPath( GraphNodeId p_nSource, GraphNodeId p_nSink,
                      std::vector<GraphNodeId>& p_roFoundAugPath )
{
    //For checking if a node is traversed or not
    std::set<GraphNodeId> oDoneNode;                 
    //Take the network as undirected graph                  
    std::vector<GraphNodeId> oSearchStack;
    oSearchStack.push_back(p_nSource);
    oDoneNode.insert(p_nSource);
    
    //Parent Mapping, for backtracking
    std::map<GraphNodeId, GraphNodeId> oParentMap;
    //oParentMap[p_nSource]   = p_nSource;
    

    GraphNodeId nParentNodeId  = INT_MIN;
    GraphNodeId nCurrentNodeId = INT_MIN;
    while ( 0 < oSearchStack.size() )
    {
        nCurrentNodeId = oSearchStack.back();
        oSearchStack.pop_back();
        oParentMap[nCurrentNodeId] = nParentNodeId;
        if ( nCurrentNodeId == p_nSink ) break;
        nParentNodeId = nCurrentNodeId;      
        
        //forward edge
        std::map<GraphNodeId,int>::iterator oSetIter;
        for ( oSetIter  = g_oFlowNetwork2[nCurrentNodeId].begin(); 
              oSetIter !=  g_oFlowNetwork2[nCurrentNodeId].end(); ++oSetIter )   
        {
            if ( oDoneNode.end() != oDoneNode.find( (*oSetIter).first )  ) 
                continue;
                  
            if ( oDataDeposite[(*oSetIter).second].m_nCapacity - oDataDeposite[(*oSetIter).second].m_nFlow <= 0 )  continue;
            
            //.first is the nodeId!! .second is the index of edge
            oSearchStack.push_back( (*oSetIter).first );
            oDoneNode.insert( (*oSetIter).first );
        }
        
        //backward edge        
        for ( std::map< GraphNodeId, std::map<GraphNodeId,int> >::iterator oIter = g_oFlowNetwork2.begin();
              oIter != g_oFlowNetwork2.end() ; ++oIter )   
        {
            if ( oDoneNode.end() != oDoneNode.find( (*oIter).first )  ) 
                continue;
  
            if ( (*oIter).second.end() != 
                  ( oSetIter = (*oIter).second.find(nCurrentNodeId)) && 
                 oDataDeposite[(*oSetIter).second].m_nFlow > 0 )
            {                 
                oSearchStack.push_back( (*oIter).first );
                oDoneNode.insert( (*oIter).first );                
            }
            

        }                 
    }// while   
    
    //start backtracking
    nCurrentNodeId = p_nSink;
    while ( oParentMap.end() != oParentMap.find(nCurrentNodeId)  ) 
    {
        p_roFoundAugPath.push_back( nCurrentNodeId );      
        nCurrentNodeId = oParentMap[nCurrentNodeId];
    }

    //semi-path is not found or it is found and calculate the augmenting flow
    int  nAugmenting    = INT_MIN;   
    if ( !oParentMap.empty() && oParentMap.end() != oParentMap.find(p_nSink) )
    {
        nAugmenting = INT_MAX;
        GraphNodeId nFrom = -1;
        GraphNodeId nTo   = -1;   
        std::map<GraphNodeId,int>::iterator oIter;
        for ( int nIdx = p_roFoundAugPath.size() - 2; nIdx >= 0; --nIdx )
        {
            nFrom = p_roFoundAugPath[nIdx+1];
            nTo   = p_roFoundAugPath[nIdx];

             g_oFlowNetwork2[nFrom].find(nTo);
            //Add to it if forward, Minus from it if backward    
            if ( g_oFlowNetwork2[nFrom].end() != 
                 ( oIter = g_oFlowNetwork2[nFrom].find(nTo) ) 
                )
            {
               if ( nAugmenting > 
                    oDataDeposite[(*oIter).second].m_nCapacity - oDataDeposite[(*oIter).second].m_nFlow 
                  )
                   nAugmenting = oDataDeposite[(*oIter).second].m_nCapacity - oDataDeposite[(*oIter).second].m_nFlow;          
            }
            else if ( g_oFlowNetwork2[nTo].end() != 
                      (oIter = g_oFlowNetwork2[nTo].find(nFrom))
                     ) 
            {
                if ( nAugmenting > oDataDeposite[(*oIter).second].m_nFlow )
                    nAugmenting = oDataDeposite[(*oIter).second].m_nFlow;
            }
            else
                assert(false);
        }
        
        assert(nAugmenting<INT_MAX);
    }    
             
#ifdef DEBUG             
    printf("==========Semi-Path ===========\n");
    for (std::vector<int>::iterator oIter = p_roFoundAugPath.begin() ; oIter != p_roFoundAugPath.end(); ++oIter)
    {
        printf("%d->", *oIter);
    }    
    printf("Aug Flow:%d\n", nAugmenting);
    printf("\n\n");            
#endif

    return nAugmenting;
}

int Ford_Fulkerson_Edmonds_Karp(GraphNodeId p_nSource, GraphNodeId p_nSink)
{
    //oAugPath goes backwards from sink
    std::vector<GraphNodeId> oAugPath;
    int nAugFlow = -1;   
    while(1)
    {
        oAugPath.clear();    
        nAugFlow = BFS_AugmentPath( p_nSource, p_nSink, oAugPath );    
        
        //no augmenting path is found
        if ( oAugPath.empty() || 
             (oAugPath.back() != p_nSource || oAugPath.front() != p_nSink) ||
             nAugFlow <= INT_MIN
           ) 
            break;
         
        //Augmenting Flow According to Semi-Path         
        GraphNodeId nFrom = -1;
        GraphNodeId nTo   = -1;   
        std::map<GraphNodeId,int>::iterator oIter;
        for ( int nIdx = oAugPath.size() - 2; nIdx >= 0; --nIdx )
        {
            nFrom = oAugPath[nIdx+1];
            nTo   = oAugPath[nIdx];

             g_oFlowNetwork2[nFrom].find(nTo);
            //Add to it if forward, Minus from it if backward 
            //Because of the property of flow network,
            //   g_oFlowNetwork2[EveryValidNodeId] will already exists
            //   (Except for sink) 
            //   the edge from source is always forward,
            //   the edge to   sink   is always forward,
            //   only the edges connecting nodes other than sink,source will be
            //   having both outgoing and ingoing flow,  
            //   so the usage of g_oFlowNetwork2[EveryValidNodeId] will not
            //   generate one new map key-value pair.
            if ( g_oFlowNetwork2[nFrom].end() != 
                 ( oIter = g_oFlowNetwork2[nFrom].find(nTo) ) 
                )
            {
                oDataDeposite[(*oIter).second].m_nFlow += nAugFlow;     
            }
            else if ( g_oFlowNetwork2[nTo].end() != 
                      (oIter = g_oFlowNetwork2[nTo].find(nFrom))
                     ) 
            {
                oDataDeposite[(*oIter).second].m_nFlow -= nAugFlow;       
            }
            else
                assert(false);
        }       
    } // while loop for augmenting process
    

    //Output Flow
    int nTotalFlow = 0;
    for ( std::map<GraphNodeId,int>::iterator oSourceIter = g_oFlowNetwork2[p_nSource].begin();
          oSourceIter != g_oFlowNetwork2[p_nSource].end();
          ++oSourceIter
         )
    {
        nTotalFlow += oDataDeposite[(*oSourceIter).second].m_nFlow;
    }    
    
    printf("Total Flow is:%d\n",nTotalFlow);
    return nTotalFlow;
}

int main(int argc, char** argv)
{
    ReadData();
//    std::vector<GraphNodeId> oAugPath;
//    BFS_AugmentPath( 0, 5, oAugPath );
    Ford_Fulkerson_Edmonds_Karp(0, 5);
    return EXIT_SUCCESS;
}
