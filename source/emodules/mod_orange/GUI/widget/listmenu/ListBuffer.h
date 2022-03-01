
#ifndef __LIST__BUFFER__H____
#define __LIST__BUFFER__H____


#include "config.h"
#include <typedef.h>
#include <fb.h>
#include <kapi.h>

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_LIST_BUFFER_MAN
{
    __u8                    mode;
    __u32                   size;
    __rectsz_t              rectsz;
    __fb_format_t           layer_format;
    void                   *buffer;
    void                   *ebuffer;    /*补充的framebuffer地址*/
    __u32                   esize;      /*补充的framebuffer大小*/
    __u32                   vsize;      /*有效的framebuffer大小*/
    __rect_t                vrect;              /*valid rect*/
} LIST_BUFFER_MAN_t;

__hdle  ListBuffer_Create(void);
__u32   ListBuffer_Destroy(__hdle listbuffer);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------



#endif //__LIST__BUFFER__H____


