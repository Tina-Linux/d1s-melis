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
#ifndef _MTPPACKET_H
#define _MTPPACKET_H
#include "mtp.h"
#include "MtpCommon.h"
#include "elibs_stdio.h"

enum
{
    MTPREQUESTPACKET = 0,
    MTPRESPONSEPACKET,
    MTPDATAPACKET,
};

struct MtpPacket
{
    uint32_t type;
    uint8_t *mBuffer;
    size_t mBufferSize;
    size_t mPacketSize;
    int (*read)(int fd, struct MtpPacket *mPacket);
    int (*write)(int fd, struct MtpPacket *mPacket);
    void (*reset)(struct MtpPacket *mPacket);
    MtpOperationCode(*getOperationCode)(struct MtpPacket *mPacket);
    void (*setOperationCode)(MtpOperationCode code, struct MtpPacket *mPacket);
    MtpTransactionID(*getTransactionID)(struct MtpPacket *mPacket);
    uint16_t (*getContainerType)(struct MtpPacket *mPacket);
    void (*setParameter)(int index, uint32_t value, struct MtpPacket *mPacket);
    int (*getParameterCount)(struct MtpPacket *mPacket);
    uint32_t (*getParameter)(int index, struct MtpPacket *mPacket);
    void (*setResponseCode)(MtpResponseCode code, struct MtpPacket *mPacket);
    MtpResponseCode(*getResponseCode)(struct MtpPacket *mPacket);
    bool (*hasData)(struct MtpPacket *mPacket);
    void (*setTransactionID)(MtpTransactionID id, struct MtpPacket *mPacket);
    void (*dump)(struct MtpPacket *mPacket);
    uint8_t *(*getData)(struct MtpPacket *mPacket);


    void (*putData8)(uint8_t value, struct MtpPacket *mPacket);
    void (*putData16)(uint16_t value, struct MtpPacket *mPacket);
    void (*putData32)(uint32_t value, struct MtpPacket *mPacket);
    void (*putData64)(uint64_t value, struct MtpPacket *mPacket);
    void (*putData128)(uint128_t value, struct MtpPacket *mPacket);
    void (*putString)(char *string, struct MtpPacket *mPacket);
    void (*putAData16)(const uint16_t *values, size_t size, struct MtpPacket *mPacket);
    void (*putAData32)(const uint32_t *values, size_t size, struct MtpPacket *mPacket);
    bool (*getUint8)(uint8_t *value, struct MtpPacket *mPacket);
    bool (*getUint16)(uint16_t *value, struct MtpPacket *mPacket);
    bool (*getUint32)(uint32_t *value, struct MtpPacket *mPacket);
    bool (*getString)(struct MtpStringBuffer *string, struct MtpPacket *mPacket);

    int mParameterCount;
    size_t mOffset;

#if 0
    union
    {
        struct RequestPacketOps
        {
            int (*read)(int fd, struct MtpPacket *mPacket);
            int (*write)(int fd, struct MtpPacket *mPacket);
        };
        struct ResponsePacketOps
        {
            int (*read)(int fd, struct MtpPacket *mPacket);
            int (*write)(int fd, struct MtpPacket *mPacket);
        };
        struct DataPacketOps
        {
            int (*read)(int fd, struct MtpPacket *mPacket);
            int (*write)(int fd, struct MtpPacket *mPacket);
        };
    };
#endif
};

struct MtpPacket *MtpPacketInit(int bufferSize, int type);
void MtpPacketRelease(struct MtpPacket *mPacket);

#endif
