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
#ifndef __MBR_H__
#define __MBR_H__

#include <mod_defs.h>

#define MAX_PART_COUNT                  8                                       // max part count
#define MBR_COPY_NUM                    4                                       // mbr backup count

#define MBR_START_ADDRESS               0x0                                     // mbr start address
#define MBR_SIZE                        1024                                    // mbr size
#define MBR_RESERVED                    (MBR_SIZE - 20 - (MAX_PART_COUNT * 64)) // mbr reserved space

// part information
typedef struct tag_PARTITION
{
    __u32 addrhi;                       // start address high 32 bit
    __u32 addrlo;                       // start address low 32 bit
    __u32 lenhi;                        // size high 32 bit
    __u32 lenlo;                        // size low 32 bit
    __u8  classname[12];                // major device name
    __u8  name[12];                     // minor device name
    __u8  res[24];                      // reserved
} PARTITION;                            // 64 bytes Ó¦¸Ã´òÉÏ pack

// mbr information
typedef struct tag_MBR
{
    __u32 crc32;                        // crc, from byte 4 to mbr tail
    __u32 version;                      // version
    __u8  magic[8];                     // magic number
    __u8  copy;                         // mbr backup count
    __u8  index;                        // current part no
    __u16 PartCount;                    // part counter
    PARTITION array[MAX_PART_COUNT];    // part info
    __u8  res[MBR_RESERVED];            // reserved space
} MBR;

#define     SUNXI_MBR_SIZE			    (16 * 1024)
#define     SUNXI_MBR_MAX_PART_COUNT	120
/* partition information */
typedef struct sunxi_partition_t
{
	unsigned  int       addrhi;
	unsigned  int       addrlo;
	unsigned  int       lenhi;
	unsigned  int       lenlo;
	unsigned  char      classname[16];
	unsigned  char      name[16];
	unsigned  int       user_type;
	unsigned  int       keydata;
	unsigned  int       ro;
	unsigned  int       sig_verify;
	unsigned  int       sig_erase;
	unsigned  int       sig_value[4];
	unsigned  int       sig_pubkey;
	unsigned  int       sig_pbumode;
	unsigned  char      reserved2[36];
}__attribute__ ((packed))sunxi_partition;

/* mbr information */
typedef struct sunxi_mbr
{
	unsigned  int       crc32;				        // crc 1k - 4
	unsigned  int       version;			        // çæ¬ä¿¡æ¯ï¼ 0x00000100
	unsigned  char 	    magic[8];			        //"softw311"
	unsigned  int 	    copy;				        //åæ°
	unsigned  int 	    index;				        //ç¬¬å ä¸ªMBRå¤ä»½
	unsigned  int       PartCount;			        //ååºä¸ªæ°
	unsigned  int       stamp[1];					//å¯¹é½
	sunxi_partition     array[SUNXI_MBR_MAX_PART_COUNT];	//
	unsigned  int       lockflag;
	unsigned  char      res[(SUNXI_MBR_SIZE - 32 - 4 - (SUNXI_MBR_MAX_PART_COUNT * sizeof(sunxi_partition)))];
}__attribute__ ((packed)) sunxi_mbr_t;

#endif // __MBR_H__