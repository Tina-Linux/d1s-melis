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
