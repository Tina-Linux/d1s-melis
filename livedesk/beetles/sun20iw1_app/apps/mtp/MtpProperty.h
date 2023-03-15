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
#ifndef _MTPPROPERTY_H
#define _MTPPROPERTY_H

#include "mtp.h"
#include "MtpPacket.h"
#include "MtpObjectInfo.h"

typedef struct
{
    union
    {
        int8_t          i8;
        uint8_t         u8;
        int16_t         i16;
        uint16_t        u16;
        int32_t         i32;
        uint32_t        u32;
        int64_t         i64;
        uint64_t        u64;
    } u;
    char               *str;
} MtpPropertyValue;

enum
{
    kFormNone = 0,
    kFormRange = 1,
    kFormEnum = 2,
    kFormDataTime = 3,
};

struct MtpProperty
{
    MtpPropertyCode mCode;
    MtpDataType mType;
    bool mWriteable;

    uint8_t mFormFlag;
    uint32_t mGroupCode;
    MtpPropertyValue mDefaultValue;
    MtpPropertyValue mCurrentValue;

    MtpPropertyValue mMinimumValue;
    MtpPropertyValue mMaximumValue;
    MtpPropertyValue mStepSize;

    void (*write)(struct MtpPacket *mData, struct MtpProperty *mProperty);

};

struct MtpProperty *MtpPropertyInit(MtpPropertyCode propCode, MtpDataType type, bool writeable, int defaultValue);
void MtpPropertyRelease(struct MtpProperty *mProperty);
MtpResponseCode getObjectPropertyList(struct MtpObjectInfo *info,
                                      uint32_t format,
                                      uint32_t property,
                                      int groupCode,
                                      int depth,
                                      struct MtpPacket *mData);
MtpResponseCode getSupportedObjectProperties(uint32_t format, struct MtpPacket *mData);

#endif
