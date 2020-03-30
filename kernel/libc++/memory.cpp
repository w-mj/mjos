#include <stdlib.h>


//void *operator new(size_t size) {
//    return malloc(size);
//}
//
//void *operator new[](size_t size)
//{
//    return malloc(size);
//}
//
//void operator delete(void *p)
//{
//    free(p);
//}
//
//void operator delete[](void *p)
//{
//    free(p);
//}

inline void *operator new(size_t, void *p)     throw() { return p; }
inline void *operator new[](size_t, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) throw() { };
inline void  operator delete[](void *, void *) throw() { };

class SomeClass {
    int a = 100;
};

SomeClass c;
