/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <typedef.h>
#include <string.h>
#include <log.h>
#include <libc.h>
#include <kapi.h>

//==============================================================================
//          ABI LIB FOR STANDARD C++
//==============================================================================

/*
************************************************************************************************************************
*                       CONSTRUCT OBJECTS FOR OBJECT ARRAY
*
*Description: construct objects for object array, the memory area of object array has been
*             allocated already.
*
*Arguments  : array_address     memory address for object array.
*             element_count     count of elements.
*             element_size      size of every element.
*             constructor       construct function.
*             destructor        destruct function.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__cxa_vec_ctor(void  *array_address,
                     __u32 element_count,
                     __u32 element_size,
                     void   *(*constructor)(void *),
                     void   *(*destructor)(void *))
{
    __u32   ix = 0;
    char    *ptr = (char *)array_address;

    if (constructor && ptr)
    {
        for (; ix != element_count; ix++, ptr += element_size)
        {
            constructor(ptr);
        }
    }

    return (void *)array_address;
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_count     count of elements.
*             element_size      size of every element.
*             padding_size      size of the additional memory for padding the class array.
*             constructor       construct function.
*             destructor        destruct function.
*             alloc             function for alloc memory.
*             dealloc           function for free memory.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__cxa_vec_new2(__u32 element_count,
                     __u32 element_size,
                     __u32 padding_size,
                     void   *(*constructor)(void *),
                     void   *(*destructor)(void *),
                     void   *(*alloc)(__u32),
                     void (*dealloc)(void *))
{
    __u32 size  = element_count * element_size + padding_size;
    char  *base = (char *)eLIBs_malloc(size);

    if (!base)
    {
        __wrn("request memory for create object failed!\n");
        return base;
    }

    if (padding_size)
    {
        base += padding_size;
        ((__u32 *)base)[-1] = element_count;
        ((__u32 *)base)[-2] = element_size;
    }

    __cxa_vec_ctor(base, element_count, element_size, constructor, destructor);
    return base;
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_count     count of elements.
*             element_size      size of every element.
*             padding_size      size of the additional memory for padding the class array.
*             constructor       construct function.
*             destructor        destruct function.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__cxa_vec_new(__u32 element_count,
                    __u32 element_size,
                    __u32 padding_size,
                    void   *(*constructor)(void *),
                    void   *(*destructor)(void *))
{
    return __cxa_vec_new2(element_count, element_size, padding_size, constructor, destructor, NULL, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_count     count of elements.
*             element_size      size of every element.
*             padding_size      size of the additional memory for padding the class array.
*             constructor       construct function.
*             destructor        destruct function.
*             alloc             function for alloc memory.
*             dealloc           function for free memory.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__cxa_vec_new3(__u32 element_count,
                     __u32 element_size,
                     __u32 padding_size,
                     void *(*constructor)(void *),
                     void *(*destructor)(void *),
                     void *(*alloc)(__u32),
                     void (*dealloc)(void *, __u32))
{
    __u32 size  = element_count * element_size + padding_size;
    char  *base = (char *)eLIBs_malloc(size);

    if (!base)
    {
        return base;
    }

    if (padding_size)
    {
        base += padding_size;
        ((__u32 *)base)[-1] = element_count;
        ((__u32 *)base)[-2] = element_size;
    }

    __cxa_vec_ctor(base, element_count, element_size, constructor, destructor);
    return base;
}


/*
************************************************************************************************************************
*                       CONSTRUCT AN ARRAY BY COPYING
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : dest_array        destination array.
*             src_array         source array.
*             element_count     count of elements.
*             element_size      size of every element.
*             constructor       construct function.
*             destructor        destruct function.
*
*Return     : destination array address.
************************************************************************************************************************
*/
void *__cxa_vec_cctor(void  *dest_array,
                      void  *src_array,
                      __u32 element_count,
                      __u32 element_size,
                      void   *(*constructor)(void *, void *),
                      void   *(*destructor)(void *))
{
    __u32   ix = 0;
    char    *dest_ptr = (char *)dest_array;
    char    *src_ptr  = (char *)src_array;

    if (constructor && dest_ptr && src_ptr)
    {
        for (; ix != element_count; ix++, src_ptr += element_size, dest_ptr += element_size)
        {
            constructor(dest_ptr, src_ptr);
        }
    }

    return (void *)dest_array;
}

/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY WITHOUT FREE MEMORY
*
*Description: destruct object array, without free memory, just destruct every object.
*
*Arguments  : array_address     array address.
*             element_count     count of elements.
*             element_size      size of every element.
*             destructor        destruct function.
*
*Return     : none.
************************************************************************************************************************
*/
void __cxa_vec_dtor(void  *array_address,
                    __u32 element_count,
                    __u32 element_size,
                    void   *(*destructor)(void *))
{
    if (destructor && array_address)
    {
        char    *ptr = (char *)array_address;
        __u32   ix   = element_count;
        ptr += element_count * element_size;

        while (ix--)
        {
            ptr -= element_size;
            destructor(ptr);
        }
    }

    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY WITHOUT FREE MEMORY
*
*Description: destruct object array, without free memory, just destruct every object.
*
*Arguments  : array_address     array address.
*             element_count     count of elements.
*             element_size      size of every element.
*             destructor        destruct function.
*
*Return     : none.
************************************************************************************************************************
*/
void __cxa_vec_cleanup(void  *array_address,
                       __u32 element_count,
                       __u32 element_size,
                       void   *(*destructor)(void *))
{
    if (destructor && array_address)
    {
        char    *ptr = (char *)array_address;
        __u32   ix   = element_count;
        ptr += element_count * element_size;

        while (ix--)
        {
            ptr -= element_size;
            destructor(ptr);
        }
    }

    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             element_size      size of every element.
*             padding_size      padding size.
*             destructor        destruct function.
*             dealloc           function for free memory.
*
*Return     : none.
************************************************************************************************************************
*/
void __cxa_vec_delete2(void  *array_address,
                       __u32 element_size,
                       __u32 padding_size,
                       void   *(*destructor)(void *),
                       void (*dealloc)(void *))
{
    char    *base = (char *)array_address;

    if (!array_address)
    {
        return;
    }

    if (padding_size)
    {
        __u32 element_count = ((__u32 *)base)[-1];
        base -= padding_size;
        __cxa_vec_dtor(array_address, element_count, element_size, destructor);
    }

    eLIBs_free(base);
    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             element_size      size of every element.
*             padding_size      padding size.
*             destructor        destruct function.
*             dealloc           function for free memory.
*
*Return     : none.
************************************************************************************************************************
*/
void __cxa_vec_delete3(void  *array_address,
                       __u32 element_size,
                       __u32 padding_size,
                       void   *(*destructor)(void *),
                       void (*dealloc)(void *, __u32))
{
    char    *base = (char *)array_address;
    __u32   size  = 0;

    if (!array_address)
    {
        return;
    }

    if (padding_size)
    {
        __u32   element_count = ((__u32 *)base)[-1];
        base -= padding_size;
        size  = element_count * element_size + padding_size;
        size  = size;
        __cxa_vec_dtor(array_address, element_count, element_size, destructor);
        eLIBs_free(base);
    }

    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             element_size      size of every element.
*             padding_size      padding size.
*             destructor        destruct function.
*
*Return     : none.
************************************************************************************************************************
*/
void __cxa_vec_delete(void  *array_address,
                      __u32 element_size,
                      __u32 padding_size,
                      void   *(*destructor)(void *))
{
    if (!array_address)
    {
        return;
    }

    __cxa_vec_delete2(array_address, element_size, padding_size, destructor, NULL);
    return;
}



//==============================================================================
//          ABI LIB FOR ARM C++
//==============================================================================


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECTS FOR OBJECT ARRAY
*
*Description: construct objects for object array, the memory area of object array has been
*             allocated already.
*
*Arguments  : array_address     memory address for object array.
*             constructor       construct function.
*             element_size      size of every element.
*             element_count     count of elements.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_ctor_nocookie_nodtor(void  *array_address,
                                       void   *(*constructor)(void *),
                                       __u32 element_size,
                                       __u32 element_count)
{
    return __cxa_vec_ctor(array_address, element_count, element_size, constructor, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECTS FOR OBJECT ARRAY
*
*Description: construct objects for object array, the memory area of object array has been
*             allocated already.
*
*Arguments  : array_address     memory address for object array.
*             constructor       construct function.
*             element_size      size of every element.
*             element_count     count of elements.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_ctor_cookie_nodtor(void  *array_address,
                                     void   *(*constructor)(void *),
                                     __u32 element_size,
                                     __u32 element_count)
{
    if (array_address == NULL)
    {
        return NULL;
    }

    array_address = ((__u32 *)array_address) + 2;
    ((__u32 *)array_address)[-2] = element_size;
    ((__u32 *)array_address)[-1] = element_count;
    return __cxa_vec_ctor(array_address, element_count, element_size, constructor, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT AN ARRAY BY COPYING
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : dest_array        destination array.
*             src_array         source array.
*             element_size      size of every element.
*             element_count     count of elements.
*             constructor       construct function.
*
*Return     : destination array address.
************************************************************************************************************************
*/
void *__aeabi_vec_cctor_nocookie_nodtor(void  *dest_array,
                                        void  *src_array,
                                        __u32 element_size,
                                        __u32 element_count,
                                        void   *(*constructor)(void *, void *))
{
    return __cxa_vec_cctor(dest_array, src_array, element_count, element_size, constructor, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_size      size of every element.
*             element_count     count of elements.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_new_cookie_noctor(__u32 element_size,
                                    __u32 element_count)
{
    return __cxa_vec_new(element_count, element_size, 2 * sizeof(__u32), NULL, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_size      size of every element.
*             element_count     count of elements.
*             constructor       construct function.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_new_nocookie(__u32 element_size,
                               __u32 element_count,
                               void   *(*constructor)(void *))
{
    return __cxa_vec_new(element_count, element_size, 0, constructor, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_size      size of every element.
*             element_count     count of elements.
*             constructor       construct function.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_new_cookie_nodtor(__u32 element_size,
                                    __u32 element_count,
                                    void   *(*constructor)(void *))
{
    return __cxa_vec_new(element_count, element_size, 2 * sizeof(__u32), constructor, NULL);
}


/*
************************************************************************************************************************
*                       CONSTRUCT OBJECT ARRAY
*
*Description: construct objects array, alloc memory, and contstruct every object.
*
*Arguments  : element_size      size of every element.
*             element_count     count of elements.
*             constructor       construct function.
*             destructor        destruct function.
*
*Return     : array address.
************************************************************************************************************************
*/
void *__aeabi_vec_new_cookie(__u32 element_size,
                             __u32 element_count,
                             void   *(*constructor)(void *),
                             void   *(*destructor)(void *))
{
    return __cxa_vec_new(element_count, element_size, 2 * sizeof(__u32), constructor, destructor);
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY WITHOUT FREE MEMORY
*
*Description: destruct object array, without free memory, just destruct every object.
*
*Arguments  : array_address     array address.
*             destructor        destruct function.
*             element_size      size of every element.
*             element_count     count of elements.
*
*Return     : none.
************************************************************************************************************************
*/
void *__aeabi_vec_dtor(void  *array_address,
                       void   *(*destructor)(void *),
                       __u32 element_size,
                       __u32 element_count)
{
    if (!array_address)
    {
        return array_address;
    }

    __cxa_vec_dtor(array_address, element_count, element_size, destructor);
    return ((__u32 *)array_address) - 2;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY WITHOUT FREE MEMORY
*
*Description: destruct object array, without free memory, just destruct every object.
*
*Arguments  : array_address     array address.
*             destructor        destruct function.
*
*Return     : none.
************************************************************************************************************************
*/
void *__aeabi_vec_dtor_cookie(void *array_address,
                              void *(*destructor)(void *))
{
    if (!array_address)
    {
        return array_address;
    }

    __cxa_vec_dtor(array_address, ((__u32 *)array_address)[-1], ((__u32 *)array_address)[-2], destructor);
    return ((__u32 *)array_address) - 2;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             destructor        destruct function.
*
*Return     : none.
************************************************************************************************************************
*/
void __aeabi_vec_delete(void *array_address,
                        void *(*destructor)(void *))
{
    if (!array_address)
    {
        return;
    }

    __cxa_vec_delete(array_address, ((__u32 *)array_address)[-2], 2 * sizeof(__u32), destructor);
    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             destructor        destruct function.
*             dealloc           function for free memory.
*
*Return     : none.
************************************************************************************************************************
*/
void __aeabi_vec_delete3(void *array_address,
                         void *(*destructor)(void *),
                         void (*dealloc)(void *, __u32))
{
    if (!array_address)
    {
        return;
    }

    __cxa_vec_delete3(array_address, ((__u32 *)array_address)[-2], 2 * sizeof(__u32), destructor, dealloc);
    return;
}


/*
************************************************************************************************************************
*                       DESTRUCT OBJECT ARRAY
*
*Description: destruct object array.
*
*Arguments  : array_address     array address.
*             dealloc           function for free memory.
*
*Return     : none.
************************************************************************************************************************
*/
void __aeabi_vec_delete3_nodtor(void *array_address,
                                void (*dealloc)(void *, __u32))
{
    if (!array_address)
    {
        return;
    }

    __cxa_vec_delete3(array_address, ((__u32 *)array_address)[-2], 2 * sizeof(__u32), NULL, dealloc);
    return;
}




typedef void (*aefuncp)(void);          /* atexit function pointer      */
typedef void (*oefuncp)(int, void *);   /* on_exit function pointer     */
typedef void (*cxaefuncp)(void *);      /* __cxa_atexit function pointer*/
typedef enum                            /* exit function types          */
{
    ef_free,
    ef_in_use,
    ef_on_exit,
    ef_cxa_atexit

} __ef_type_t;


struct exit_function
{
    long int type;                      /* enum ef_type                 */
    union
    {
        struct
        {
            oefuncp func;
            void    *arg;
        } on_exit;
        struct
        {
            cxaefuncp   func;
            void *arg;
            void *dso_handle;
        } cxa_atexit;
    } funcs;
    struct exit_function    *next;
};


//exit function list
struct exit_function    *__exit_function_list = 0;


/*
*********************************************************************************************************
*                       ALLOCATE AN ITEM FROM THE EXIT FUNCTION TABLE
*
*Description: Register a function to be called by exit or when a shared library
*             is unloaded..
*
*Arguments  : arg   .
*
*Return     : array address.
*********************************************************************************************************
*/
static struct exit_function *__new_exitfn(void)
{
    struct exit_function    *efp;
    //request memory for new exit function
    efp = eLIBs_malloc(sizeof(struct exit_function));

    if (!efp)
    {
        __wrn("request memory for exit function failed!\n");
        return 0;
    }

    memset(efp, 0, sizeof(struct exit_function));
    //put the item into the function list head
    esKRNL_SchedLock();
    efp->next = __exit_function_list;
    __exit_function_list = efp;
    esKRNL_SchedUnlock();
    return efp;
}


/*
************************************************************************************************************************
*                       REGISTER AN EXIT FUNCTION
*
*Description: Register a function to be called by exit or when a shared library
*             is unloaded..
*
*Arguments  : func          function pointer;
*             arg           parameter for fucntion;
*             dso_handle    don't care;
*
*Return     : result.
************************************************************************************************************************
*/
int __cxa_atexit(cxaefuncp func, void *arg, void *dso_handle)
{
    struct exit_function    *efp;

    if (!func)
    {
        //none function, need do nothing
        return 0;
    }

    //try to get a function entry
    efp = __new_exitfn();

    if (!efp)
    {
        __wrn("try to get a new exit function entry failed!\n");
        return -1;
    }

    esKRNL_SchedLock();
    efp->funcs.cxa_atexit.func       = func;
    efp->funcs.cxa_atexit.arg        = arg;
    efp->funcs.cxa_atexit.dso_handle = dso_handle;
    efp->type = ef_cxa_atexit;
    esKRNL_SchedUnlock();
    return 0;
}


/*
************************************************************************************************************************
*                       REGISTER AN EXIT FUNCTION
*
*Description: Register a function to be called by exit or when a shared library
*             is unloaded..
*
*Arguments  : arg   parameter for function;
*             func  function entry;
*             d     don't care;
*
*Return     : result.
************************************************************************************************************************
*/
int __aeabi_atexit(void *arg, void (*func)(void *), void *d)
{
    return __cxa_atexit(func, arg, d);
}


/*
************************************************************************************************************************
*                       CLEAR EXIT FUNCTION
*
*Description: clear exit function in exit function table.
*
*Arguments  : dso_handle    handler, don't care;
*
*Return     : none.
*
*Note       : if dso_handle is non-NULL, call all functions registered with '__cxa_atexit'
*             with the same dso handle.  Otherwise, if dso_handle is NULL, call all of the
*             registered handlers.
************************************************************************************************************************
*/
void __cxa_finalize(void *dso_handle)
{
    struct exit_function    *efp = __exit_function_list;

    //care the clear order
    while (efp)
    {
        __exit_function_list = efp->next;
        (*efp->funcs.cxa_atexit.func)(efp->funcs.cxa_atexit.arg);
        //free memory of the item
        eLIBs_free(efp);
        efp = __exit_function_list;
    }

    return;
}
