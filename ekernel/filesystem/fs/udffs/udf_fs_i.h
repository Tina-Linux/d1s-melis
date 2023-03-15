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
#ifndef __UDF_FS_I_H__
#define __UDF_FS_I_H__

#define UDF_I_LOCATION(X)   ( UDF_I(X)->i_location )
#define UDF_I_LENEATTR(X)   ( UDF_I(X)->i_lenEAttr )
#define UDF_I_LENALLOC(X)   ( UDF_I(X)->i_lenAlloc )
#define UDF_I_LENEXTENTS(X) ( UDF_I(X)->i_lenExtents )
#define UDF_I_UNIQUE(X)     ( UDF_I(X)->i_unique )
#define UDF_I_ALLOCTYPE(X)  ( UDF_I(X)->i_alloc_type )
#define UDF_I_EFE(X)        ( UDF_I(X)->i_efe )
#define UDF_I_USE(X)        ( UDF_I(X)->i_use )
#define UDF_I_STRAT4096(X)  ( UDF_I(X)->i_strat4096 )
#define UDF_I_NEXT_ALLOC_BLOCK(X)   ( UDF_I(X)->i_next_alloc_block )
#define UDF_I_NEXT_ALLOC_GOAL(X)    ( UDF_I(X)->i_next_alloc_goal )
#define UDF_I_CRTIME(X)     ( UDF_I(X)->i_crtime )
#define UDF_I_SAD(X)        ( UDF_I(X)->i_ext.i_sad )
#define UDF_I_LAD(X)        ( UDF_I(X)->i_ext.i_lad )
#define UDF_I_DATA(X)       ( UDF_I(X)->i_ext.i_data )

struct udf_inode_info
{
    struct timespec     i_crtime;
    /* Physical address of inode */
    struct kernel_lb_addr       i_location;
    __u64           i_unique;
    __u32           i_lenEAttr;
    __u32           i_lenAlloc;
    __u64           i_lenExtents;
    __u32           i_next_alloc_block;
    __u32           i_next_alloc_goal;
    unsigned        i_alloc_type : 3;
    unsigned        i_efe : 1;  /* extendedFileEntry */
    unsigned        i_use : 1;  /* unallocSpaceEntry */
    unsigned        i_strat4096 : 1;
    unsigned        reserved : 26;
    union
    {
        struct short_ad *i_sad;
        struct long_ad      *i_lad;
        __u8        *i_data;
    } i_ext;
    struct inode vfs_inode;
};

static inline struct udf_inode_info *UDF_I(struct inode *inode)
{
    return list_entry(inode, struct udf_inode_info, vfs_inode);
}



#endif /* __UDF_FS_I_H__ */