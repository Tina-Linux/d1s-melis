
#ifndef __LIST__BUFFER__C____
#define __LIST__BUFFER__C____

#include "config.h"
#include "ListBuffer.h"
#include "sysmemory.h"

//------------------------------------------------------------------------------------------------------------
//创建ListBuffer并分配空间
//------------------------------------------------------------------------------------------------------------
__hdle ListBuffer_Create(void)
{
    LIST_BUFFER_MAN_t  *pList_Buffer;
    pList_Buffer = (LIST_BUFFER_MAN_t *)MEM_MALLOC(sizeof(LIST_BUFFER_MAN_t));

    if (!pList_Buffer)
    {
        return NULL;
    }

    return (__hdle)pList_Buffer;
}

//------------------------------------------------------------------------------------------------------------
//销毁ListBuffer并回收空间
//------------------------------------------------------------------------------------------------------------
__u32 ListBuffer_Destroy(__hdle listbuffer)
{
    LIST_BUFFER_MAN_t  *pList_Buffer;
    pList_Buffer = (LIST_BUFFER_MAN_t *)listbuffer;

    if (listbuffer)
    {
        MEM_FREE(pList_Buffer);
    }
    else
    {
        return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------



#endif //__LIST__BUFFER__C____


