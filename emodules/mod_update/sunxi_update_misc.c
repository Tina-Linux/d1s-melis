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
#include "sunxi_update_misc.h"
#define STAMP_VALUE             0x5F0A6C39
typedef struct tag_CRC32_DATA
{
	__u32 CRC;             
	__u32 CRC_32_Tbl[256];
} CRC32_DATA_t;

__u32 add_sum(void *buffer, __u32 length)
{
    unsigned int *buf;
    unsigned int count;
    unsigned int sum = 0;
    count = length >> 2;                         // 以 字（4bytes）为单位计数
    sum = 0;
    buf = (unsigned int *)buffer;

    while (count--)
    {
        sum += *buf++;                         // 依次累加，求得校验和
    }

    switch (length & 0x03)
    {
        case 0:
            return sum;

        case 1:
            sum += (*buf & 0x000000ff);
            break;

        case 2:
            sum += (*buf & 0x0000ffff);
            break;

        case 3:
            sum += (*buf & 0x00ffffff);
            break;
    }

    return sum;
}

__u32 calc_crc32(const unsigned char *buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;
	__u32 CRC32 = 0xffffffff;  /* 设置初始值 */
	crc32.CRC = 0;

	for ( i = 0; i < 256; ++i) /* 用++i以提高效率 */
	{
		crc32.CRC = i;
		for ( j = 0; j < 8 ; ++j)
		{
			if (crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; /* 设置初始值 */
    for ( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32 >> 8);
    }

	return CRC32^0xffffffff;
}
uint sunxi_generate_checksum(void *buffer, uint length, uint div, uint src_sum)
{
	uint *buf;
	int count;
	uint sum;

	count = length >> 2;
	sum   = 0;
	buf   = (__u32 *)buffer;
	do {
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= (4*div)) > (4 - 1));

	while (count-- > 0)
		sum += *buf++;

	sum = sum - src_sum + STAMP_VALUE;

	return sum;
}


int sunxi_verify_checksum(void *buffer, uint length, uint src_sum)
{
	uint sum;
	sum = sunxi_generate_checksum(buffer, length, 1, src_sum);

	eLIBs_printf("src sum=%x, check sum=%x\n", src_sum, sum);
	if (sum == src_sum)
		return 0;
	else
		return -1;
}

