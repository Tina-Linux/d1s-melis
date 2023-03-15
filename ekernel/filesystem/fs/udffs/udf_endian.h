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
#ifndef __UDF_ENDIAN_H__
#define __UDF_ENDIAN_H__

#include    "endians.h"

static inline struct kernel_lb_addr lelb_to_cpu(struct lb_addr in)
{
    struct kernel_lb_addr out;

    out.logicalBlockNum = le32_to_cpu(in.logicalBlockNum);
    out.partitionReferenceNum = le16_to_cpu(in.partitionReferenceNum);

    return out;
}

static inline struct lb_addr cpu_to_lelb(struct kernel_lb_addr in)
{
    struct lb_addr out;

    out.logicalBlockNum = cpu_to_le32(in.logicalBlockNum);
    out.partitionReferenceNum = cpu_to_le16(in.partitionReferenceNum);

    return out;
}

static inline struct short_ad lesa_to_cpu(struct short_ad in)
{
    struct short_ad out;

    out.extLength = le32_to_cpu(in.extLength);
    out.extPosition = le32_to_cpu(in.extPosition);

    return out;
}

static inline struct short_ad cpu_to_lesa(struct short_ad in)
{
    struct short_ad out;

    out.extLength = cpu_to_le32(in.extLength);
    out.extPosition = cpu_to_le32(in.extPosition);

    return out;
}

static inline struct kernel_long_ad lela_to_cpu(struct long_ad in)
{
    struct kernel_long_ad out;

    out.extLength = le32_to_cpu(in.extLength);
    out.extLocation = lelb_to_cpu(in.extLocation);

    return out;
}

static inline struct long_ad cpu_to_lela(struct kernel_long_ad in)
{
    struct long_ad out;

    out.extLength = cpu_to_le32(in.extLength);
    out.extLocation = cpu_to_lelb(in.extLocation);

    return out;
}

static inline struct kernel_extent_ad leea_to_cpu(struct extent_ad in)
{
    struct kernel_extent_ad out;

    out.extLength = le32_to_cpu(in.extLength);
    out.extLocation = le32_to_cpu(in.extLocation);

    return out;
}

#endif  /* __UDF_ENDIAN_H__ */