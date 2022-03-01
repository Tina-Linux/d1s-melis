/*
************************************************************************************************************************
*                                                        BITMAP
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : rect_op.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.07.20
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.07.20       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __rect_op_h
#define  __rect_op_h


extern void   rect_copy(const RECT *dst_pos,
                        void       *dst_base,
                        const RECT *src_pos,
                        void       *src_base,
                        const SIZE *rect_size,
                        __u32       byte_count);
extern void   rect_copy_alpha(const RECT *dst_pos,
                              void       *dst_base,
                              const RECT *src_pos,
                              void       *src_base,
                              const SIZE *rect_size,
                              __u32       byte_count);
extern void   fill_rect_with_column(void *dst_base, const SIZE *dst_size_p, const RECT *valid_rect_p,
                                    const void *column_buf, __u32 byte_count);
extern void   fill_rect_with_row(void *dst_base, const SIZE *dst_size_p, const RECT *valid_rect_p,
                                 const void *row_buf, __u32 byte_count);
extern void   rect_hori_mirror(void *buf, __u32 width, __u32 height, __u32 byte_count);
extern void   rect_vert_mirror(void *buf, __u32 width, __u32 height, __u32 byte_count);
extern void   rect_get_row(const void *src_base, __u32 width, __u32 height, __u32 y, void *row_buf, __u32 byte_count);
extern void   rect_get_column(const void *src_base, __u32 width, __u32 height, __u32 x, void *column_buf, __u32 byte_count);



#endif     //  ifndef __rect_op_h

/* end of rect_op.h  */
