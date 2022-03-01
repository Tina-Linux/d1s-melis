/*
--------------------------------------------------------------------------------
    modified by kevin for porting c++ library
--------------------------------------------------------------------------------
*/
#ifndef __NEW__
#define __NEW__
#include <typedef.h>

/* normal operator new */
inline void *operator new (unsigned int size)
{
    if (!size)
    {
        size = 1;
    }

    return (void *)malloc(size);
}
/* normal operator delete*/
inline void operator delete (void *p)
{
    if (p)
    {
        free(p);
    }

    return;
}


/* placement new */
inline void *operator new (unsigned int size, void *ptr)
{
    return ptr;
}
/* placement delete */
inline void operator delete (void *, void *)
{
    return;
}


/* array new */
inline void *operator new[](unsigned int size)
{
    if (!size)
    {
        size = 1;
    }

    return (void *)malloc(size);
};
/* array delete */
inline void operator delete[](void *p)
{
    if (p)
    {
        free(p);
    }

    return;
};


/* placement array new */
inline void *operator new[](unsigned int size, void *ptr)
{
    return ptr;
}
/* placement array delete */
inline void operator delete[](void *, void *)
{
    return;
}

#endif /*__NEW__ */

/* End of new */
