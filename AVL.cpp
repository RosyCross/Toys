#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <assert.h>
#include <time.h>
#include <algorithm>

class Node
{
    public:
        //Node(): key(-1),bf(0), LeftChild(NULL), Get(NULL),Parent(NULL) {};   
        Node(): m_nKey(-1) {};   
        int GetKey() const { return m_nKey; } ;
        int SetKey( int p_nNewKey ) { m_nKey = p_nNewKey; }               
    
    private:
        //Node(const Node&  rhs) {};
       Node& operator=(const Node& rhs) {};
       
        int   m_nKey;          
};
//Node* g_pNode = NULL;  //TESTING CODE        
//======================================================================
template <class T, class functor>
void PostOrderTraverse( T* p_rCurrent, functor p_pOperator)
{
     
    if (  NULL !=  p_rCurrent->GetLeftChild() )
    {
        PostOrderTraverse ( p_rCurrent->GetLeftChild(), p_pOperator); 
    }
    
    if ( NULL != p_rCurrent->GetRightChild() )                  
    {
        PostOrderTraverse ( p_rCurrent->GetRightChild(), p_pOperator); 
    }
    
    p_pOperator( p_rCurrent );
    
}
//======================================================================
template <class T, class functor>
void InOrderTraverse( T& p_rCurrent, functor p_pOperator)
{    
     
    if ( NULL != p_rCurrent->GetLeftChild() )
    {
        //printf("LC \n"); 
        InOrderTraverse ( p_rCurrent->GetLeftChild(), p_pOperator); 
    }
    
    //printf("BF ");
    p_pOperator( p_rCurrent ); 
    //printf("AF ");    
    
    if ( NULL != p_rCurrent->GetRightChild() )                  
    {
        //printf("RC \n");  
        InOrderTraverse ( p_rCurrent->GetRightChild(), p_pOperator); 
    }            
}
//======================================================================
template <class T, class functor>
void InOrderTraverse2( T& p_rCurrent, T& p_rAnswer, functor p_pOperator)
{    
     
    if ( NULL != p_rCurrent->GetLeftChild() )
    {
        //printf("LC \n"); 
        InOrderTraverse2 ( p_rCurrent->GetLeftChild(), p_rAnswer, p_pOperator); 
    }
    
    //printf("BF ");
    p_pOperator( p_rCurrent , p_rAnswer); 
    //printf("AF ");    
    
    if ( NULL != p_rCurrent->GetRightChild() )                  
    {
        //printf("RC \n");  
        InOrderTraverse2 ( p_rCurrent->GetRightChild(), p_rAnswer, p_pOperator); 
    }            
}
//============================== Functors ========================//
template <class T>
static
void PrintAndDelete( T& p_rElement )
{
    //printf("TypeInfo: %s\n", typeid(T*).name() );
    
    if ( NULL == p_rElement ) return;
     
    //printf("Delete Key: %d\n", p_rElement->key);
    delete p_rElement;
    p_rElement = NULL;
    
     
}
//================================================================//
template <class T>
static 
void PrintIt(T& p_prElement)
{
     printf("Element Key:%d\n", p_prElement->GetUserData()->GetKey() );     
}           
//================================================================//
template <class T>
static 
void FindDeleteCase1( T& p_prElement, T& p_prAnswer )
{
    if ( NULL == p_prElement || NULL != p_prAnswer ) return;
    
    if ( 1 == p_prElement->m_nBf && 0 == p_prElement->GetLeftChild()->m_nBf )
        p_prAnswer = p_prElement->GetRightChild();
}

template <class T>
static 
void FindDeleteCase2( T& p_prElement, T& p_prAnswer )
{
    if ( NULL == p_prElement || NULL != p_prAnswer ) return;
    
    if ( -1 == p_prElement->m_nBf && 0 == p_prElement->GetRightChild()->m_nBf )
        p_prAnswer = p_prElement->GetLeftChild();
}
//================================================================//
template <class T>
static
bool CheckHeightDiff( T& p_rElement )
{
    //Check if the height different is within the SPEC:
    // | HL - HR | < -2
    if ( NULL == p_rElement /*|| 
        (NULL == p_rElement->Get && NULL == p_rElement->LeftChild )*/ ) 
        return true;
    

    int height_diff =  CalHeight( p_rElement->GetLeftChild() ) - CalHeight(p_rElement->GetRightChild() );
    if ( height_diff > -2 && height_diff < 2 )
    {
         printf("NODE PASSED:%d, height difference:%d\n",
                 p_rElement->GetUserData()->GetKey(), height_diff);

         return true;        
    }
    else
    {
         printf("NODE FAILED:%d, height difference:%d\n",
                 p_rElement->GetUserData()->GetKey(), height_diff);
    }         
    

    return false;
}

template <class T>
static
int CalHeight( T& p_rElement )
{
    if ( NULL == p_rElement ) return -1;    

    return (1 + 
            std::max( CalHeight( p_rElement->GetLeftChild() ) , CalHeight( p_rElement->GetRightChild() ) )  
            );                        
}

//=======================================================================//
template <class T>
struct DefaultComparator_S
{
    bool operator() (const T& p_oObj1, const T& p_oObj2)
    {
        return p_oObj1.GetKey() < p_oObj2.GetKey();
    }
};
//=======================================================================//
template <class T>
class AVL 
{
      public:
          class TreeNode_C
          {
              public:
                  friend class AVL;
                   TreeNode_C( const T& p_roUserData ):m_pRightChild(NULL),m_pLeftChild(NULL),m_pParent(NULL)
                               ,m_nKey(-1),m_nBf(0),m_pUserData( new T(p_roUserData) ) {} ;
                  ~TreeNode_C() { delete m_pUserData; m_pUserData = NULL; } ;

                const      T* GetUserData() const { return m_pUserData; } ;
                   
             TreeNode_C*& GetLeftChild()   { return m_pLeftChild; };     
             TreeNode_C*& GetRightChild()  { return m_pRightChild; };
             TreeNode_C*& GetParent() { return m_pParent; };

                  
              private:
                   TreeNode_C(const TreeNode_C&);
                   TreeNode_C& operator=(const TreeNode_C&);
                                          
                  TreeNode_C*  m_pRightChild;
                  TreeNode_C*  m_pLeftChild;
                  TreeNode_C*  m_pParent;
                         int   m_nKey;
                         public:
                         int   m_nBf;
                         private:
                           T*  m_pUserData;
                         //  T   m_oUserData;  

          };
          
             
      public:
          AVL():m_Root(NULL) {printf("AVL::CTOR\n");}       
         ~AVL() {Init(); printf("AVL::DTOR\n");}       
    
          void Init();
TreeNode_C*  Insert( const T& newElement ) 
          {
              bool bTaller = false;
              return iInsert(newElement, NULL, m_Root, bTaller, DefaultComparator_S<T>() ) ;
          }
          
          template<class Comparator > 
TreeNode_C*  Insert( const T& newElement, Comparator p_oFunctor ) 
          {
              bool bTaller = false;
              return iInsert(newElement, NULL, m_Root, bTaller, p_oFunctor);
          } 

TreeNode_C*  Delete( const T& deleteElement ) 
          {
              bool bShorter = false;
              return iDelete(deleteElement, NULL, m_Root, bShorter); 
          } 
          
          
TreeNode_C*  FindMax() { return iFindMax( GetRoot() ); }
TreeNode_C*  FindMin() { return iFindMin( GetRoot() ); }
TreeNode_C*  FindPredecessor(TreeNode_C* const& p_rpInput)
             { return iFindPredecessor( p_rpInput );   }
             
TreeNode_C*  FindSuccessor(TreeNode_C* const& p_rpInput)
             { return iFindSuccessor( p_rpInput );     }  

          
          
 TreeNode_C*&   GetRoot() { return m_Root; };
                
          //FOR TESTING AND DEBUG
         T*  GetUserData( TreeNode_C* p_pTreeNode )
         {
             if ( NULL != p_pTreeNode )
                 return p_pTreeNode->m_pUserData;
             else
                 return NULL;          
         };     
             
      private:  
               
        
         
#if 0    
         //will use iterative mathod            
          T*   iFindMax(T* const& p_pRoot);
          T*   iFindMin(T* const& p_pRoot);
          T*   iFindPredecessor(T* const& p_pInput);
          T*   iFindSuccessor(T* const& p_pInput);

          template<class Comparator>      
          T* iInsert( const T& newElement, T* parentPtr, T*& currentPtr , bool& bTaller, Comparator p_oFunctor);           
          //Search Element, then delete. (NOT DIRECT DELETION)
          T*   iDelete( const T& deleteElement, T* parentPtr, T*& currentPtr , bool& bShorter);           
          T*   iDeleteNode( T*& p_pParentNode, T*& p_pDeleteNode );

          void iL_Rotation(  T*& currentPtr );
          void iLL_Rotation( T*& currentPtr );
          void iLR_Rotation( T*& currentPtr );
          
          void iR_Rotation(  T*& currentPtr );
          void iRR_Rotation( T*& currentPtr );
          void iRL_Rotation( T*& currentPtr );
#endif          
 
 TreeNode_C*   iFindMax(TreeNode_C* const& p_pRoot);
 TreeNode_C*   iFindMin(TreeNode_C* const& p_pRoot);
 TreeNode_C*   iFindPredecessor(TreeNode_C* const& p_pInput);
 TreeNode_C*   iFindSuccessor(TreeNode_C* const& p_pInput);

 template<class Comparator>      
 TreeNode_C*   iInsert( const T& newElement, TreeNode_C* parentPtr, TreeNode_C*& currentPtr , bool& bTaller, Comparator p_oFunctor);           
          //Search Element, then delete. (NOT DIRECT DELETION)
 TreeNode_C*   iDelete( const TreeNode_C& deleteElement, TreeNode_C* parentPtr, TreeNode_C*& currentPtr , bool& bShorter);           
 TreeNode_C*   iDeleteNode( TreeNode_C*& p_pParentNode, TreeNode_C*& p_pDeleteNode );
     
     
          void iL_Rotation(  TreeNode_C*& currentPtr );
          void iLL_Rotation( TreeNode_C*& currentPtr );
          void iLR_Rotation( TreeNode_C*& currentPtr );
          
          void iR_Rotation(  TreeNode_C*& currentPtr );
          void iRR_Rotation( TreeNode_C*& currentPtr );
          void iRL_Rotation( TreeNode_C*& currentPtr );
          
//          T* m_Root;
          TreeNode_C* m_Root;          
};
AVL<Node> b;
AVL<Node>::TreeNode_C* g_pNode = NULL;  //TESTING CODE        

//=============================================================
template<class T>
void AVL<T>::Init()
{
    if ( NULL == m_Root ) return;

    PostOrderTraverse( m_Root, PrintAndDelete<TreeNode_C*> );         
    m_Root = NULL; 
}
//=============================================================
// 
// !! Delete Node having at most one child !!
//
template<class T>
typename AVL<T>::TreeNode_C*  
AVL<T>::iDeleteNode( TreeNode_C*& p_pParentNode, TreeNode_C*& p_rpDeleteNode )
{
    if ( NULL == p_rpDeleteNode ) return NULL;

    //Find correct replacement for the delete node
    TreeNode_C* pRtv = p_rpDeleteNode;
    TreeNode_C* pRepNode = NULL;
    if ( NULL != p_rpDeleteNode->m_pLeftChild )
    {
        pRepNode = p_rpDeleteNode->m_pLeftChild;        
        //printf("promte from LC\n");
    }
    else if ( NULL != p_rpDeleteNode->m_pRightChild ) 
    {
        pRepNode = p_rpDeleteNode->m_pRightChild;
        //printf("promte from RC\n");
    }
    else
    {
        //printf("Have two Children\n");
    }

    //Promote up and adjust parent
    p_rpDeleteNode = pRepNode;    
    if ( NULL != pRepNode )
    {        
        pRepNode->m_pParent = pRtv->m_pParent;
    }    
    
    //Remove All parent child relation in order to make deleted node isolated
    pRtv->m_pLeftChild  = NULL;
    pRtv->m_pRightChild = NULL;    
    pRtv->m_pParent     = NULL;
    
    return pRtv;
}
//=============================================================
template<class T>
typename AVL<T>::TreeNode_C* 
AVL<T>::iDelete( const TreeNode_C& deleteElement, TreeNode_C* parentPtr, TreeNode_C*& currentPtr, bool& bShorter)
{
    if ( NULL == currentPtr ) { bShorter = false; return NULL; };
    
    //TESTING CODE 
    DefaultComparator_S<Node> oFunctor;
    
    TreeNode_C* pReturnPtr = NULL;
    //if ( deleteElement.key < currentPtr->key )
    if ( oFunctor( *(deleteElement.GetUserData()) , *(currentPtr->GetUserData()) ) )
    {
        pReturnPtr = iDelete (deleteElement, currentPtr, currentPtr->m_pLeftChild, bShorter);
        if ( bShorter )
        {
            //++currentPtr->m_nBf;       
            --currentPtr->m_nBf;       
            if ( -1 > currentPtr->m_nBf )
            {
                iR_Rotation( currentPtr );
                bShorter = false;          
            }
        }    

        (0 == currentPtr->m_nBf) ? bShorter = false : 1;
    }     
    //else if ( deleteElement.key > currentPtr->key )
    else if ( oFunctor( *(currentPtr->GetUserData()), *(deleteElement.GetUserData()) ) )
    {
        pReturnPtr = iDelete (deleteElement, currentPtr, currentPtr->m_pRightChild, bShorter); 
        if ( bShorter )
        {
            //--currentPtr->m_nBf; 
            ++currentPtr->m_nBf; 
            if ( 1 < currentPtr->m_nBf )
            {
                iL_Rotation( currentPtr );
                bShorter = false;          
            }
        }    

        (0 == currentPtr->m_nBf) ? bShorter = false : 1;
    }
    else //TARGET FOUND
    {
        pReturnPtr = currentPtr;
        if ( NULL == currentPtr->m_pLeftChild || 
             NULL == currentPtr->m_pRightChild )
        {   // HAS ZERO or ONE Child  
            //printf("BF Have zero or one child\n");
            pReturnPtr = iDeleteNode(parentPtr, currentPtr);
            bShorter = true;
        }    
        else // MUST HAVE TWO CHILDREN
        {
            //printf("BF Have two child\n"); 
            TreeNode_C* pPredecessor = iFindPredecessor(currentPtr);
            //printf("BF CHECKING: %p %p - %p %p\n",pReturnPtr,pPredecessor,
            //       pReturnPtr->GetUserData(),pPredecessor->GetUserData());
            assert( NULL != pPredecessor );
            
            //Now, pReturnPtr == pPredecessor;
            pReturnPtr = iDelete( *pPredecessor, parentPtr, currentPtr, bShorter );
            /*
            printf("AF CHECKING:  %p %p\n",pReturnPtr,pPredecessor );
            printf("AF CHECKING2: %p %p %p\n",pPredecessor,
                                           currentPtr->m_pLeftChild, 
                                           currentPtr->m_pRightChild);
            */                              
            assert( NULL != pReturnPtr && pReturnPtr == pPredecessor );
            //Splice out currentPtr
            // Beware of the special case:
            //           A
            //          / \
            //         B   C
            pReturnPtr = currentPtr;
            pPredecessor->m_pParent     = currentPtr->m_pParent;
            //if ( currentPtr->m_pLeftChild != pPredecessor )
               pPredecessor->m_pLeftChild  = currentPtr->m_pLeftChild;
            //else
            //   pPredecessor->m_pLeftChild  = NULL;
            
            //if ( currentPtr->m_pRightChild != pPredecessor )      
                pPredecessor->m_pRightChild = currentPtr->m_pRightChild;
            //else
            //    pPredecessor->m_pRightChild = NULL;  
            
            currentPtr = pPredecessor;
            //printf("AF IS SHORTER?:%d\b", bShorter);          
            //bShorter = false;
            
        }                        
    }

    //if ( 0 == currentPtr->m_nBf )
    //    bShorter = false;    

    return pReturnPtr;
} 
//=============================================================
template<class T>
template<class Comparator>
typename AVL<T>::TreeNode_C* 
AVL<T>::iInsert( const T& newElement, TreeNode_C* parentPtr, TreeNode_C*& currentPtr, 
                 bool& bTaller, Comparator p_oFunctor = DefaultComparator_S<T>())
{
    //printf("currentPtr: %p\n", currentPtr );
    
    if ( NULL == currentPtr )
    {
        currentPtr = new TreeNode_C( newElement );               
        assert(NULL != currentPtr);
        currentPtr->m_pParent = parentPtr;

        //currentPtr = new T();
        //assert(NULL != currentPtr);
        //currentPtr->key = newElement.key;
        //currentPtr->Parent = parentPtr;         
    
        bTaller = true;
        //TESTING CODE!
        g_pNode = currentPtr;
        return currentPtr;
    }
    //else if ( newElement.key < currentPtr->key )
    else if ( p_oFunctor( newElement, *currentPtr->GetUserData() ) )
    {
        //Insert to Left 
        //printf("Insert to Left\n");
        iInsert (newElement, currentPtr, currentPtr->m_pLeftChild, bTaller, p_oFunctor);
        if ( bTaller )
        {
            ++currentPtr->m_nBf;       
            if ( 1 < currentPtr->m_nBf )
            {
                iL_Rotation( currentPtr );
                bTaller = false;          
            }
        }    
    }     
    //else if ( newElement.key > currentPtr->key )
    else if ( p_oFunctor( *currentPtr->GetUserData(), newElement  ) )
    {
        //Insert to Right
//        printf("Insert to Right\n");        
        iInsert (newElement, currentPtr, currentPtr->m_pRightChild, bTaller, p_oFunctor); 
        if ( bTaller )
        {
            --currentPtr->m_nBf; 
            if ( -1 > currentPtr->m_nBf )
            {
                iR_Rotation( currentPtr );
                bTaller = false;          
            }
        }    

    }
    else
    {    
        //printf("INSERT EXISTED NODE: insert:%d existed:%d\n",newElement.GetKey(),currentPtr->GetUserData()->GetKey() );  
    }

    if ( 0 == currentPtr->m_nBf )
        bTaller = false;    

    return NULL;
}           
//================================================================
template<class T>
void AVL<T>::iL_Rotation ( TreeNode_C*& currentPtr )
{
    assert( NULL != currentPtr && NULL != currentPtr->m_pLeftChild );
    if ( currentPtr->m_pLeftChild->m_nBf == 1 )     
    {
        iLL_Rotation( currentPtr );  
        //printf("LL\n");
    }     
    else if ( currentPtr->m_pLeftChild->m_nBf == -1 )
    {
        iLR_Rotation( currentPtr );     
        //printf("LR\n");
    }
    else
    {
        iLL_Rotation( currentPtr );  
        //assert(false);
    }   
}
//=================================================================
template<class T>
void AVL<T>::iLL_Rotation( TreeNode_C*& currentPtr )
{
    assert( NULL != currentPtr && NULL != currentPtr->m_pLeftChild ); 

    //Backup currentPtr leftchild, for adjustment
    TreeNode_C* currentPtrLC = currentPtr->m_pLeftChild;
    //For Parent Adjustment, backup
    TreeNode_C* currentPtrParent = currentPtr->m_pParent;
    
    //change currentPtr's left child to be the right child of its current
    //Leftchild's right child 
    currentPtr->m_pLeftChild = currentPtrLC->m_pRightChild;
    currentPtr->m_nBf = 0;
    //currentPtr->Parent = currentPtrLC;       //parent adjustment
    
    //shift up currentPtr's leftchild to be the root of the subtree
    currentPtrLC->m_pRightChild = currentPtr;
    currentPtrLC->m_nBf = 0;   
    //currentPtrLC->Parent = currentPtrParent; // parent adjustment
        
    currentPtr = currentPtrLC;    
    
    //Parent Adjustment
    currentPtr->m_pParent             = currentPtrParent;
    currentPtr->m_pRightChild->m_pParent = currentPtr;
    if ( NULL != currentPtr->m_pRightChild->m_pLeftChild )
        currentPtr->m_pRightChild->m_pLeftChild->m_pParent = currentPtr->m_pRightChild; 
}
//=================================================================
template <class T>
void AVL<T>::iLR_Rotation( TreeNode_C*& currentPtr )
{
    assert( NULL != currentPtr && NULL != currentPtr->m_pLeftChild && NULL != currentPtr->m_pLeftChild->m_pRightChild );      

    TreeNode_C* currentPtrLC = currentPtr->m_pLeftChild; 
    //Backup Parent of currentPtr for parent adjustment
    TreeNode_C* currentPtrParent = currentPtr->m_pParent;   
    
    // BEWARE: 
    // Change the value of balance factor of currentPtrLCRC after the rotation 
    // process is over   
    // And its value should be -1 or +1 during the rotation and will be zero 
    // after rotation.
    currentPtr->m_pLeftChild = currentPtrLC->m_pRightChild->m_pRightChild;
    
    TreeNode_C* currentPtrLCRC = currentPtrLC->m_pRightChild;
    currentPtrLC->m_pRightChild = currentPtrLC->m_pRightChild->m_pLeftChild;
    switch ( currentPtrLCRC->m_nBf )
    {
        case 1:
            currentPtr->m_nBf = -1; 
            currentPtrLC->m_nBf = 0;
            break;
        case 0: // means no children for currnetPtrLCRC
            currentPtr->m_nBf = 0;   
            currentPtrLC->m_nBf = 0;
            break;
        case -1:
            currentPtr->m_nBf = 0;
            currentPtrLC->m_nBf = 1;
            break;
        default:
            assert(false);                       
    };
    currentPtrLCRC->m_pLeftChild  = currentPtrLC;
    currentPtrLCRC->m_pRightChild = currentPtr; // ADDED
    currentPtrLCRC->m_nBf = 0;     
    
    currentPtr = currentPtrLCRC;
    
    //Adjust Parent
    currentPtr->m_pParent             = currentPtrParent; 
    currentPtr->m_pLeftChild->m_pParent  = currentPtr;
    currentPtr->m_pRightChild->m_pParent = currentPtr;
    if ( NULL != currentPtr->m_pRightChild->m_pLeftChild )
        currentPtr->m_pRightChild->m_pLeftChild->m_pParent = currentPtr->m_pRightChild;  
    if ( NULL != currentPtr->m_pLeftChild->m_pRightChild )
        currentPtr->m_pLeftChild->m_pRightChild->m_pParent = currentPtr->m_pLeftChild;
}
//==================================================================
template <class T>
void AVL<T>::iR_Rotation( TreeNode_C*& currentPtr )
{
    assert( NULL != currentPtr ); 
    if ( -1 == currentPtr->m_pRightChild->m_nBf )
    {
        iRR_Rotation( currentPtr );     
        //printf("RR\n");
    }
    else if ( 1 == currentPtr->m_pRightChild->m_nBf )
    {
        iRL_Rotation( currentPtr );
        //printf("RL\n");
    }
    else
    {
        iRR_Rotation( currentPtr );     
        //assert(false);
    }    
}
//==================================================================
template <class T>
void AVL<T>::iRR_Rotation( TreeNode_C*& currentPtr )
{
    assert( NULL != currentPtr && NULL != currentPtr->m_pRightChild ); 
   
    TreeNode_C* currentPtrRC = currentPtr->m_pRightChild;
    //Backup currentPtr's parent for parent adjustment
    TreeNode_C* currentPtrParent = currentPtr->m_pParent;
    
    currentPtr->m_pRightChild = currentPtrRC->m_pLeftChild;
    currentPtr->m_nBf = 0;
//    currentPtr->Parent = currentPtrRC;        // parent Adjustment
    
    currentPtrRC->m_pLeftChild = currentPtr;
    currentPtrRC->m_nBf = 0;
//    currentPtrRC->Parent = currentPtrParent; // parent Adjustment
    
    currentPtr = currentPtrRC;
    
    //Parent Adjustment
    currentPtr->m_pParent             = currentPtrParent;
    currentPtr->m_pLeftChild->m_pParent = currentPtr;
    if ( NULL != currentPtr->m_pLeftChild->m_pRightChild )
        currentPtr->m_pLeftChild->m_pRightChild->m_pParent = currentPtr->m_pLeftChild; 

}
//===================================================================
template <class T>
void AVL<T>::iRL_Rotation( TreeNode_C*& currentPtr )
{
     assert( NULL != currentPtr && NULL != currentPtr->m_pRightChild && currentPtr->m_pRightChild->m_pLeftChild);
     
     TreeNode_C* currentPtrRC = currentPtr->m_pRightChild;
     //Backup currentPtr's parent for parent adjustment
     TreeNode_C* currentPtrParent = currentPtr->m_pParent;
     
     currentPtr->m_pRightChild = currentPtrRC->m_pLeftChild->m_pLeftChild;

     TreeNode_C* currentPtrRCLC = currentPtrRC->m_pLeftChild;
     currentPtrRC->m_pLeftChild = currentPtrRC->m_pLeftChild->m_pRightChild;
     //currentPtrRC->m_nBf = -( 1 == currentPtrRCLC->m_nBf ? 1:0);
     switch ( currentPtrRCLC->m_nBf )
     {
         case 1:
             currentPtr->m_nBf = 0;
             currentPtrRC->m_nBf = -1;
             break;
         case 0:
             currentPtr->m_nBf = 0;
             currentPtrRC->m_nBf = 0;
             break;
         case -1:
             currentPtr->m_nBf = 1;
             currentPtrRC->m_nBf = 0;
             break;         
         default:
             assert(false);                           
     }       
     currentPtrRCLC->m_pRightChild = currentPtrRC;
     currentPtrRCLC->m_pLeftChild  = currentPtr; // ADDED
     currentPtrRCLC->m_nBf = 0;
     
     currentPtr = currentPtrRCLC;
     
     //Parent Adjustment
     currentPtr->m_pParent                = currentPtrParent;
     currentPtr->m_pLeftChild->m_pParent  = currentPtr;
     currentPtr->m_pRightChild->m_pParent  = currentPtr;
     if ( NULL != currentPtr->m_pLeftChild->m_pRightChild )
         currentPtr->m_pLeftChild->m_pRightChild->m_pParent = currentPtr->m_pLeftChild;  
     if ( NULL != currentPtr->m_pRightChild->m_pLeftChild )
         currentPtr->m_pRightChild->m_pLeftChild->m_pParent = currentPtr->m_pRightChild;

}
//===================================================================
template<class T>
typename AVL<T>::TreeNode_C* 
AVL<T>::iFindMax(TreeNode_C* const& p_pRoot)
{
    if ( NULL == p_pRoot ) return NULL;
                          
    //Use ITERATIVE METHOD
    TreeNode_C* pAnwser = p_pRoot;
    while ( NULL != pAnwser->GetRightChild() )
    {
        pAnwser = pAnwser->GetRightChild();      
    }      

    assert(NULL == pAnwser->GetRightChild());       
    return pAnwser;                        
}
//====================================================================
template<class T>
typename AVL<T>::TreeNode_C* 
AVL<T>::iFindMin(TreeNode_C* const& p_pRoot)
{
    if ( NULL == p_pRoot ) return NULL;
                          
    //Use ITERATIVE METHOD
    TreeNode_C* pAnwser = p_pRoot;
    while ( NULL != pAnwser->m_pLeftChild )
    {
        pAnwser = pAnwser->m_pLeftChild;      
    }      
 
    assert(NULL == pAnwser->m_pLeftChild);   
    return pAnwser;                                                  
}
//===================================================================          
template<class T>
typename AVL<T>::TreeNode_C* 
AVL<T>::iFindPredecessor(TreeNode_C* const& p_rpInput)
{
    if ( NULL == p_rpInput ) return NULL;
    
    TreeNode_C* pPredecessor = p_rpInput;//wiil NOT BE NULL
    if ( NULL != p_rpInput->m_pLeftChild )                  
    {
        pPredecessor = iFindMax( p_rpInput->m_pLeftChild ); 
    }
    else
    {                                 
        while ( NULL != pPredecessor->m_pParent &&
                pPredecessor->m_pParent->m_pLeftChild == pPredecessor
                )  
        {   pPredecessor = pPredecessor->m_pParent;   }

        pPredecessor = pPredecessor->m_pParent;        
    }
     
    return pPredecessor;
}
//===================================================================          
template<class T>
typename AVL<T>::TreeNode_C* 
AVL<T>::iFindSuccessor(TreeNode_C* const& p_rpInput)
{
    if ( NULL == p_rpInput ) return NULL;
    
    TreeNode_C* pSuccessor = p_rpInput;                             
    if ( NULL != p_rpInput->GetRightChild() )
    {
        pSuccessor = iFindMin( p_rpInput->GetRightChild() );     
    }
    else
    {
        while ( NULL != pSuccessor->m_pParent &&
                pSuccessor->m_pParent->m_pRightChild == pSuccessor                
              )
        { pSuccessor = pSuccessor->m_pParent; }
        pSuccessor = pSuccessor->m_pParent;
    }

    return pSuccessor;
}
//===================================================================          
template <class T>
class RefToValue
{  
public:
    RefToValue(T& ref) : ref_(ref)
    {}
    RefToValue(const RefToValue& rhs) : ref_(rhs.ref_)
    {}
    operator T& () const
    {
        printf("OPERATOR T&\n");     
        return ref_;
    }
private:
    // Disable - not implemented
    RefToValue();
    RefToValue& operator=(const RefToValue&);
       
    T& ref_;
};

int test(Node& p_roANODE)
{
    printf("HAHA\n");
    return 1;
}


int test2(Node& p_roANODE)
{   
    printf("TEST2 \n");
    p_roANODE.SetKey( 1234 );
    return 1;
}



int main(int argc, char** argv)
{
//    const Node bb; test(bb);
Node bb;
RefToValue<Node>  g(bb);
test2(g);
printf("AFTER TEST2 KEY VALUE:%d\n",bb.GetKey());
    {
        srand(time(NULL));       
     
        Node testNode;     
        AVL<Node> avlTree;        
        avlTree.Init();
#if 0        
        testNode.key = 1;
        avlTree.Insert(testNode);
        testNode.key = 2;       
        avlTree.Insert(testNode);        
        testNode.key = 3;
        avlTree.Insert(testNode);
#if 0 
        testNode.key = 4;
        avlTree.Insert(testNode);
#endif        
        testNode.key = 6;
        avlTree.Insert(testNode);
#endif

#if 0
        testNode.key = 2868;
        avlTree.Insert(testNode);
        printf("\n");
        testNode.key = 3419;
        avlTree.Insert(testNode);
        printf("\n");
        testNode.key = 447;
        avlTree.Insert(testNode);
        printf("\n");
        testNode.key = 7813;
        avlTree.Insert(testNode);
        printf("\n");
        testNode.key = 4960;
        avlTree.Insert(testNode);
        printf("\n");
        testNode.key = 95;
        avlTree.Insert(testNode);
        printf("\n");        
        testNode.key = 9508;
        avlTree.Insert(testNode);        
        printf("\n");        
        testNode.key = 634;
        avlTree.Insert(testNode);        
        printf("\n");        
        testNode.key = 8083;
        avlTree.Insert(testNode);
        printf("\n");        
        
        testNode.key = 5803;
        avlTree.Insert(testNode);
        
#endif
        
#if 1
        for ( int nIdx = 0 ;  nIdx < 50; ++nIdx )
        {
            int a = ( rand()% 10000 ); printf("Node Key = %d\n",a);
            testNode.SetKey( a );
            avlTree.Insert(testNode);
        }            
        
    
        InOrderTraverse( avlTree.GetRoot(), PrintIt<AVL<Node>::TreeNode_C*> );

        printf("\n Start Checking Heights of Each Node \n");
        InOrderTraverse( avlTree.GetRoot(), CheckHeightDiff<AVL<Node>::TreeNode_C*> );
        
        Node* pMaxNode = avlTree.GetUserData( avlTree.FindMax() );
        Node* pMinNode = avlTree.GetUserData( avlTree.FindMin() );
        printf("Max:%d, Min:%d\n", 
                (NULL != pMaxNode)? pMaxNode->GetKey() : INT_MAX, 
                (NULL != pMinNode)? pMinNode->GetKey() : INT_MIN
                 );
        printf("\n");
        
        Node* pPredec = avlTree.GetUserData( avlTree.FindPredecessor(g_pNode) );        
        printf("%d Predecessor is :%d\n",
               (NULL != g_pNode)? g_pNode->GetUserData()->GetKey() : INT_MIN,
               (NULL != pPredec)? pPredec->GetKey() : INT_MIN
               );

        Node* pSuccessor = avlTree.GetUserData( avlTree.FindSuccessor(g_pNode) );
        printf("%d Successor is :%d\n",
               (NULL != g_pNode)? g_pNode->GetUserData()->GetKey() : INT_MIN,
               (NULL != pSuccessor)? pSuccessor->GetKey() : INT_MIN
               );
#endif

        
        printf("\n\n========= START Deletion Test ============= \n" );
        AVL<Node>::TreeNode_C* deleteNode1 = g_pNode->GetParent();
        AVL<Node>::TreeNode_C* deleteNode2 = g_pNode->GetLeftChild();
        //spacial search, intend to make assertion fail
        deleteNode1 = NULL;
        
        InOrderTraverse2( avlTree.GetRoot(), deleteNode1, FindDeleteCase2<AVL<Node>::TreeNode_C*> );
        
        if ( NULL == deleteNode1 )
        {             
            deleteNode1 = avlTree.GetRoot(); 
            int times = (rand()% 5) + 1;     
            printf("Choose Candidates From Right Child of Root:%d\n",times);
            for ( int nIdx = 0; nIdx < times; ++nIdx);
                deleteNode1 = deleteNode1->GetRightChild();
        }    
        if ( NULL == deleteNode2 )
        {
            deleteNode2 = avlTree.GetRoot(); 
            int times = (rand()% 5) + 1;     
            printf("Choose Candidates From Left Child of Root:%d\n",times);
            for ( int nIdx = 0; nIdx < times; ++nIdx);
                deleteNode2 = deleteNode2->GetLeftChild();
        }    

        
        printf("\n\n Deletion Test: delete target => %d %d\n",
               (NULL != deleteNode1)? deleteNode1->GetUserData()->GetKey() : INT_MIN, 
               (NULL != deleteNode2)? deleteNode2->GetUserData()->GetKey() : INT_MIN);
        printf("Pointers node/userdata %x/%x %x/%x\n", 
                deleteNode1, deleteNode1->GetUserData(),
                deleteNode2, deleteNode2->GetUserData() );               
               
        printf("================ Deletion 1 ============== \n");       
        avlTree.Delete(*(deleteNode1->GetUserData()));
        printf("================ Deletion 2 ============== \n");
        avlTree.Delete(*(deleteNode2->GetUserData()));
        
        printf("\n Start Checking Heights of Each Node \n");
        InOrderTraverse( avlTree.GetRoot(), CheckHeightDiff<AVL<Node>::TreeNode_C*> );
        
        printf("\n\n\n\nRe-search Predecessor and Successor\n"); 
        pPredec = avlTree.GetUserData( avlTree.FindPredecessor(g_pNode) );        
        printf("%d Predecessor is :%d\n",
               (NULL != g_pNode)? g_pNode->GetUserData()->GetKey() : INT_MIN,
               (NULL != pPredec)? pPredec->GetKey() : INT_MIN
               );

        pSuccessor = avlTree.GetUserData( avlTree.FindSuccessor(g_pNode) );
        printf("%d Successor is :%d\n",
               (NULL != g_pNode)? g_pNode->GetUserData()->GetKey() : INT_MIN,
               (NULL != pSuccessor)? pSuccessor->GetKey() : INT_MIN
               );          
//        InOrderTraverse( avlTree.GetRoot(), PrintIt<AVL<Node>::TreeNode_C*> );
        //delete g_pNode;     
    }    
    double a = 123456710.0;
    long b = (long)a;
    printf("b:%ld\n",b); 
    system("PAUSE");
    return EXIT_SUCCESS;    
}
