#ifndef _LAMBDA_H_
#define _LAMBDA_H_

//debug function
#define STRLIZE_(str) #str
#define MYPRINT_(a,b) printf("%s==>%s\n", a, STRLIZE_(b)); 

//add prefix MY to prevent name collision. But it is hard to prevent name
//collision for macro names.
//The following should be used in this file only
#define MYCOMBINE_(a,b) a ## b
#define MYLCN_(a,suffix) MYCOMBINE_(a,suffix)


// ============= the body of Lambda utility ==============
// OVERVIEW:
//    It is sometimes annoying to write an extra function elsewhere when
//    we just want to do trivial things, e.g. print the value of each elements....
//    Or sometimes we need some short functions to work with a template function but
//    the name of the member of the various types hinders us to reuse the generic code because
//    A PROTOCAL between the generic code writer and code user must be established. and what if    
//    you are using a third party code that cannot be altered?
//    For example, if I have a generic in-order traversal code for trees, I want to reuse it.
//    but how do I access to the left and right child(what is the name for it)?
//    If we can write short functions easil, the generic code can be:
//    template<typename T, typename L, typename R>
//    inorder(T* node, L leftChild, R rightChild)
//    {
//       inorder( leftChild(node) );
//       ..... do your thing
//       inorder( rightChild(node) );
//    }
//
//    So the concept of Lambda function comes into my mind. Although the newest c++ supports this
//    functionality but there is still plenty of people using old compiler. so before the total
//    transition, this gadget may help.
//
// Technique:
//    1. limit the scope of the lambda function.
//    2. users can declare many lambda functions in one user function.
//
//    so first, inject the code into a LOCAL calss that has a static member function. The name
//    of the class is :  NOTSEEN+__LINE__ (<--line number).
//    __LINE__ is for preventing name collision.
//
//    the parameters are named by: _1, _2, _3.......
//    REASON: c++ does not allow local function and local class cannot be used as a parameter for
//            template function.

// Limitation:
//    1. the delcaration of two lambda functions cannot be in the same line.
//    2. only supports lambda function. lambda object is not supported because of the behavior of c++ template
//    2. if want to have lambda function that supports more parameters, please hand-write it. For now,
//       there is no automatic way for it.
//
// STEPS
// 1. inject code into a LOCAL class that has a static member function.
//    REASON: c++ does not allow to define local functions.
//
//    The first, second , third parameter... and so on is named by:
//    _1, _2, _3...... This is the extra knowledge that requires users to know.
//    
// 2. define a function pointer type according to the macro parameter
// 3. declare a LOCAL variable of function pointer type of step2 and assign
//    the address of the static member function to it.
// 4. the name of the LOCAL variable holding the address of static member function is decided
//    by the parameter of the macro. this allows user to control the variable name and
//    reduce the need for extra knowledge.
//================================
#define Lambda_(R,P1,code, suffix) \
struct MYCOMBINE_(NOTSEEN, suffix) \
{ \
static R __Lambda(P1& _1) \
{ \
   code; \
} \      
};

#define LambdaFunc_(varName, R,P1,code) \
Lambda_(R,P1,code, __LINE__); \
typedef  R (*MYLCN_(funcPtr,__LINE__))(P1&); \
MYLCN_(funcPtr,__LINE__) varName = MYLCN_(NOTSEEN , __LINE__)::__Lambda;


#endif
