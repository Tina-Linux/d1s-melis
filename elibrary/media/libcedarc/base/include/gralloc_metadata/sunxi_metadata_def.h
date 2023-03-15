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
#ifndef __SUNXI_METADATA_DEF_H__
#define __SUNXI_METADATA_DEF_H__

#include <stdint.h>

enum {
    /* hdr static metadata is available */
    SUNXI_METADATA_FLAG_HDR_SATIC_METADATA   = 0x00000001,
    /* hdr dynamic metadata is available */
    SUNXI_METADATA_FLAG_HDR_DYNAMIC_METADATA = 0x00000002,

    /* afbc header data is available */
    SUNXI_METADATA_FLAG_AFBC_HEADER          = 0x00000010,
};

struct display_master_data {

    /* display primaries */
    uint16_t display_primaries_x[3];
    uint16_t display_primaries_y[3];

    /* white_point */
    uint16_t white_point_x;
    uint16_t white_point_y;

    /* max/min display mastering luminance */
    uint32_t max_display_mastering_luminance;
    uint32_t min_display_mastering_luminance;

};

/* static metadata type 1 */
struct hdr_static_metadata {
    struct display_master_data disp_master;

    uint16_t maximum_content_light_level;//* maxCLL
    uint16_t maximum_frame_average_light_level;//* maxFALL
};

struct afbc_header {
    uint32_t signature;
    uint16_t filehdr_size;
    uint16_t version;
    uint32_t body_size;
    uint8_t ncomponents;
    uint8_t header_layout;
    uint8_t yuv_transform;
    uint8_t block_split;
    uint8_t inputbits[4];
    uint16_t block_width;
    uint16_t block_height;
    uint16_t width;
    uint16_t height;
    uint8_t left_crop;
    uint8_t top_crop;
    uint16_t block_layout;
};

/* sunxi metadata for ve and de */
struct sunxi_metadata {
    struct hdr_static_metadata hdr_smetada;
    struct afbc_header afbc_head;

    // flag must be at last always
    uint32_t flag;
};

#endif /* __SUNXI_METADATA_DEF_H__ */
