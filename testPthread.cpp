#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

template<typename T, typename F>
struct CallCmd
{
    T* obj;
    F  funcPtr;
};

template<typename T>
void* CallWrapper(void* input)
{
    if ( NULL == input) return NULL;
    ((((T*)input)->obj)->*((T*)input)->funcPtr)();

    return NULL;
}

class MyClass
{
    public:
        void selfPrint() { printf("MyClass:%p\n", this); }
};
//typedef void (MyClass::*cPtr)();
//template void CallWrapper< CallCmd<MyClass,cPtr> >(void*);

int main(int argc, char** argv)
{
    printf("HAHA\n");
    //typeof(&MyClass::selfPrint) ag;
    typedef void (MyClass::*cPtr)();
    CallCmd< MyClass, cPtr > aa,bb,cc;
    //template void CallWrapper< CallCmd<MyClass,cPtr> >(void*);
#if 1 
    MyClass a,b,c;
    aa.obj = &a, aa.funcPtr = &MyClass::selfPrint;
    bb.obj = &b, bb.funcPtr = &MyClass::selfPrint;
    cc.obj = &c, cc.funcPtr = &MyClass::selfPrint;
#if 0
    (a.*aa.funcPtr)();
    (b.*bb.funcPtr)();
    (c.*cc.funcPtr)();
    (aa.obj->*aa.funcPtr) ();    
#endif
#endif
    pthread_t t1, t2, t3;
    pthread_create(&t1,NULL,CallWrapper< CallCmd<MyClass,cPtr> > , &aa);
    pthread_create(&t2,NULL,CallWrapper< CallCmd<MyClass,cPtr> > , &bb);
    pthread_create(&t3,NULL,CallWrapper< CallCmd<MyClass,cPtr> > , &cc);

    int rtv = EXIT_FAILURE;
    void *rtvp = &rtv;
    pthread_join(t1, &rtvp);
    pthread_join(t2, &rtvp);
    pthread_join(t3, &rtvp);
    return EXIT_SUCCESS;
}
