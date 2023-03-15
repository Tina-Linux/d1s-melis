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
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "fatfs.h"
#include <log.h>

#if FSYS_FAT_FORMAT_EN

#define FSYS_FAT_SEC_SIZE 512

/* BIOS parameter block (FAT12/FAT16) */
typedef struct __FSYS_FAT_BPB
{
    __u32               TotSec32;         /* RSVD + FAT + ROOT + FATA (>=64k) */
    __u32               FATSz32;          /* number of FAT sectors          */
    __u32               RootClus;         /* root dir clus for FAT32        */
    __u16               BytesPerSec;      /* _512_,1024,2048,4096           */
    __u16               RootEntCnt;       /* number of root dir entries     */
    __u16               TotSec16;         /* RSVD + FAT + ROOT + FATA (<64k) */
    __u16               FATSz16;          /* number of FAT sectors          */
    __u16               ExtFlags;         /* mirroring info                 */
    __u16               FSInfo;           /* position of FSInfo structure   */
    __u16               Signature;        /* 0xAA55 Signature               */
    __u16               RsvdSecCnt;       /* 1 for FAT12 & FAT16            */
    unsigned char       SecPerClus;       /* sec in allocation unit         */
    unsigned char       NumFATs;          /* 2                              */
} __fsys_fat_BPB_t;

/*
**********************************************************************************************************************
*             #define constants
**********************************************************************************************************************
*/
#define FSYS_KNOWNMEDIA_NUM   sizeof(_fsys_wd_format_media_table) / sizeof(__fsys_wd_format_media_t)

#ifndef FSYS_FAT_NOFORMAT
#define FSYS_FAT_NOFORMAT       0
#endif

#ifndef FSYS_FAT_DISKINFO
#define FS_FAT_DISKINFO       1
#endif

#ifndef FSYS_SUPPORT_SEC_ACCESS
#define FSYS_SUPPORT_SEC_ACCESS 1
#endif

/*
**********************************************************************************************************************
*             Local data types
**********************************************************************************************************************
*/

#if (FSYS_FAT_NOFORMAT==0)

typedef struct
{
    __s32   media_id;
    __u32   totsec32;
    __u32   hiddsec;
    __u16   totsec16;
    __u16   rootentcnt;
    __u16   fatsz16;
    __u16   secpertrk;
    __u16   numheads;
    char    secperclus;
    char    media;
    char    fsystype;
} __fsys_wd_format_media_t;


typedef struct
{
    __u32   SecNum;
    __u32   Num;
} _FS_FAT_ROOTENTCNT ;


typedef struct
{
    __u32 SecNum;
    __u16 Num;
} _FS_FAT_SECPERCLUST;


/*
**********************************************************************************************************************
*             Local Variables
**********************************************************************************************************************
*/

/* table for getting number of root entries for a given media size */

static const _FS_FAT_ROOTENTCNT _FS_auto_rootcnt[] =
{
    {         0x100,     0x40 },
    {         0x200,     0x80 },
    {      0x0b40UL,     0xe0 },
    {  0x0001f3c9UL,    0x100 },
    {  0xffffffffUL,    0x200 }
};

/* table for calculating cluster size */
static const _FS_FAT_SECPERCLUST _FS_auto_secperclust[] =
{
    /* medias up to 512MB are formatted with FAT16 */
    {     0x0b40UL, 0x0001 },
    {      32680UL, 0x0002 },
    {     262144UL, 0x0004 },
    {     524288UL, 0x0008 },
    {    1048576UL, 0x0010 },
    /* medias bigger than 512MB are formatted with FAT32 */
    {   16777216UL, 0x0008 },
    {   33554432UL, 0x0010 },
    {   67108864UL, 0x0020 },
    { 0xffffffffUL, 0x0040 }
};

#endif /* FS_FAT_NOFORMAT==0 */


/*
**********************************************************************************************************************
*             Local functions
**********************************************************************************************************************
*/

/*
**********************************************************************************************************************
*             #define constants
**********************************************************************************************************************
*/

#ifndef FSYS_FAT_NOFAT32
#define FSYS_FAT_NOFAT32        0
#endif /* FS_FAT_NOFAT32 */

int format_write_sec(const void *pData, __u32 Sector, __u32 N, __hdle pPart)
{
    static int progress = 0;

    if (!(progress++ & 0x3f))
    {
        __inf(">");
    }

    return esFSYS_pwrite(pData, Sector, N, pPart);
}

/*
**********************************************************************************************************************
*
*             _FS_fat_format
*
  Description:
  FS internal function. Format a media using specified parameters.
  Currently this function needs many parameters. The function will be
  improved.

  Parameters:
  pDriver     - Pointer to a device driver function table.
  Unit        - Unit number.
  SecPerClus  - Number of sector per allocation unit.
  RootEntCnt  - For FAT12/FAT16, this is the number of 32 byte root
                directory entries. 0 for FAT32.
  TotSec16    - 16-bit total count of sectors. If zero, TotSec32 must
                be none-zero.
  TotSec32    - 32-bit total count of sectors. If zero, TotSec16 must
                be none-zero.
  Media       - Media byte.
  FATSz16     - 16-bit count of sectors occupied by one FAT. 0 for
                FAT32 volumes, which use FATSz32.
  FATSz32     - 32-bit count of sectors occupied by one FAT. This is
                valid for FAT32 medias only.
  SecPerTrk   - Sectors per track.
  NumHeads    - Number of heads.
  HiddSec     - Count of hidden sectors preceding the partition.
  FSysType    - ==0 => FAT12
                ==1 => FAT16
                ==2 => FAT32

  Return value:
  >=0         - Media has been formatted.
  <0          - An error has occured.
**********************************************************************************************************************
*/
static int _fsys_fat_format(__hdle pPart,
                            char    SecPerClus,
                            __u16   RootEntCnt,
                            __u16   TotSec16,
                            __u32   TotSec32,
                            char    Media,
                            __u16   FATSz16,
                            __u32   FATSz32,
                            __u16   SecPerTrk,
                            __u16   NumHeads,
                            __u32   HiddSec,
                            char    FSysType,
                            char   *VolLab)
{
    __fsys_fat_BPB_t bpb;
    __u32 sector;
    __u32 value;
    char *buffer;
    int i;
    int j;
    int n;

    buffer = palloc(FSYS_FAT_SEC_SIZE);
    if (!buffer)
    {
        return -1;
    }
    memset(buffer, 0x00, (__u32)FSYS_FAT_SEC_SIZE);
    /* Setup BPB */
    memset(&bpb, 0x00, (__u32)sizeof(bpb));
    bpb.BytesPerSec = 0x0200;       /* _512_,1024,2048,4096           */
    bpb.SecPerClus  = SecPerClus;   /* sec in allocation unit         */
    if (FSysType != 2)
    {
        bpb.RsvdSecCnt  = 0x0001;       /* 1 for FAT12 & FAT16            */
    }
#if (FSYS_FAT_NOFAT32==0)
    else
    {
        bpb.RsvdSecCnt  = 0x0020;       /* 32 for FAT32                   */
    }
#else
    /* FAT32 disabled */
    else
    {
        pfree(buffer, FSYS_FAT_SEC_SIZE);
        return -1;
    }
#endif /* FS_FAT_NOFAT32==0 */
    bpb.NumFATs     = 0x02;         /* 2                              */
    bpb.RootEntCnt  = RootEntCnt;   /* number of root dir entries     */
    bpb.TotSec16    = TotSec16;     /* RSVD+FAT+ROOT+DATA (<64k)      */
    bpb.FATSz16     = FATSz16;      /* number of FAT sectors          */
    bpb.TotSec32    = TotSec32;     /* RSVD+FAT+ROOT+FATA (>=64k)     */
    bpb.Signature   = 0xaa55;       /* 0xAA55 Signature               */

    /* setup BPB specifics for FAT32 */
    bpb.FATSz32     = FATSz32;      /* number of FAT sectors          */
    bpb.ExtFlags    = 0x0000;       /* mirroring info                 */
    bpb.RootClus    = 0x00000002UL; /* root dir clus for FAT32        */
    bpb.FSInfo      = 0x0001;       /* position of FSInfo structure   */

    /*
        Prepare buffer with information of the BPB
        offset 0 - 35 is same for FAT12/FAT16 and FAT32
    */

    /* jmpBoot = 0xe9 0x0000 */
    buffer[0]   = (char)0xe9;
    buffer[1]   = (char)0x00;
    buffer[2]   = (char)0x00;
    /* OEMName = '        ' */
    for (i = 3; i < 11; i++)
    {
        buffer[i] = (char)0x20;
    }
    /* BytesPerSec */
    buffer[11]  = (char)(bpb.BytesPerSec & 0xff);
    buffer[12]  = (char)((unsigned int)(bpb.BytesPerSec & 0xff00) >> 8);
    /* SecPerClus */
    buffer[13]  = (char)bpb.SecPerClus;
    /* RsvdSecCnt */
    buffer[14]  = (char)(bpb.RsvdSecCnt & 0xff);
    buffer[15]  = (char)((unsigned int)(bpb.RsvdSecCnt & 0xff00) >> 8);
    /* NumFATs */
    buffer[16]  = (char)bpb.NumFATs;
    /* RootEntCnt */
    buffer[17]  = (char)(bpb.RootEntCnt & 0xff);
    buffer[18]  = (char)((unsigned int)(bpb.RootEntCnt & 0xff00) >> 8);
    /* TotSec16 */
    buffer[19]  = (char)(bpb.TotSec16 & 0xff);
    buffer[20]  = (char)((unsigned int)(bpb.TotSec16 & 0xff00) >> 8);
    /* Media */
    buffer[21]  = Media;
    /* FATSz16 */
    buffer[22]  = (char)(bpb.FATSz16 & 0xff);
    buffer[23]  = (char)((unsigned int)(bpb.FATSz16 & 0xff00) >> 8);
    /* SecPerTrk */
    buffer[24]  = (char)(SecPerTrk & 0xff);
    buffer[25]  = (char)((unsigned int)(SecPerTrk & 0xff00) >> 8);
    /* NumHeads */
    buffer[26]  = (char)(NumHeads & 0xff);
    buffer[27]  = (char)((unsigned int)(NumHeads & 0xff00) >> 8);
    /* HiddSec */
    buffer[28]  = (char)(HiddSec & 0xff);
    buffer[29]  = (char)((__u32)(HiddSec & 0x0000ff00UL) >> 8);
    buffer[30]  = (char)((__u32)(HiddSec & 0x00ff0000UL) >> 16);
    buffer[31]  = (char)((__u32)(HiddSec & 0xff000000UL) >> 24);
    /* TotSec32 */
    buffer[32]  = (char)(bpb.TotSec32 & 0xff);
    buffer[33]  = (char)((__u32)(bpb.TotSec32 & 0x0000ff00UL) >> 8);
    buffer[34]  = (char)((__u32)(bpb.TotSec32 & 0x00ff0000UL) >> 16);
    buffer[35]  = (char)((__u32)(bpb.TotSec32 & 0xff000000UL) >> 24);

    /* Offset 36 and above have different meanings for FAT12/FAT16 and FAT32 */
    if (FSysType != 2)
    {
        /* FAT12/FAT16 */
        /* DrvNum = 0x00 (floppy) */
        buffer[36]  = (char)0x00;
        /* Reserved1 = 0x00 (floppy) */
        buffer[37]  = (char)0x00;
        /* BootSig = 0x00 (next three fields are not 'present') */
        buffer[38]  = (char)0x00;
        /* VolID = 0x00000000 (serial number, e.g. date/time) */
        for (i = 39; i < 43; i++)
        {
            buffer[i] = (char)0x00;
        }
        /* VolLab = ' ' */
        for (i = 43; i < 54; i++)
        {
            buffer[i] = (char)0x20;
        }
        if (VolLab)
        {
            strncpy(&buffer[43], VolLab, 11);
        }
        /* FilSysType = 'FAT12' */
        if (FSysType == 0)
        {
            memcpy(&buffer[54], "FAT12   ", 8);
            fs_log_info("fs type is %s\nstart format...", "FAT12");
        }
        else
        {
            memcpy(&buffer[54], "FAT16   ", 8);
            fs_log_info("fs type is %s\nstart format...", "FAT16");
        }
    }
#if (FSYS_FAT_NOFAT32==0)
    else
    {
        /* FAT32 */
        /* FATSz32 */
        buffer[36]  = (char)(bpb.FATSz32 & 0xff);
        buffer[37]  = (char)((__u32)(bpb.FATSz32 & 0x0000ff00UL) >> 8);
        buffer[38]  = (char)((__u32)(bpb.FATSz32 & 0x00ff0000UL) >> 16);
        buffer[39]  = (char)((__u32)(bpb.FATSz32 & 0xff000000UL) >> 24);
        /* EXTFlags */
        buffer[40]  = (char)(bpb.ExtFlags & 0xff);
        buffer[41]  = (char)((unsigned int)(bpb.ExtFlags & 0xff00) >> 8);
        /* FSVer = 0:0 */
        buffer[42]  = 0x00;
        buffer[43]  = 0x00;
        /* RootClus */
        buffer[44]  = (char)(bpb.RootClus & 0xff);
        buffer[45]  = (char)((__u32)(bpb.RootClus & 0x0000ff00UL) >> 8);
        buffer[46]  = (char)((__u32)(bpb.RootClus & 0x00ff0000UL) >> 16);
        buffer[47]  = (char)((__u32)(bpb.RootClus & 0xff000000UL) >> 24);
        /* FSInfo */
        buffer[48]  = (char)(bpb.FSInfo & 0xff);
        buffer[49]  = (char)((unsigned int)(bpb.FSInfo & 0xff00) >> 8);
        /* BkBootSec = 0x0006; */
        buffer[50]  = 0x06;
        buffer[51]  = 0x00;
        /* Reserved */
        for (i = 52; i < 64; i++)
        {
            buffer[i] = (char)0x00;
        }
        /* DrvNum = 0x00 (floppy) */
        buffer[64]  = (char)0x00;
        /* Reserved1 = 0x00 (floppy) */
        buffer[65]  = (char)0x00;
        /* BootSig = 0x00 (next three fields are not 'present') */
        buffer[66]  = (char)0x00;
        /* VolID = 0x00000000 (serial number, e.g. date/time) */
        for (i = 67; i < 71; i++)
        {
            buffer[i] = (char)0x00;
        }
        /* VolLab = ' ' */
        for (i = 71; i < 82; i++)
        {
            buffer[i] = (char)0x20;
        }
        if (VolLab)
        {
            strncpy(&buffer[71], VolLab, 11);
        }
        /* FilSysType = 'FAT12' */
        memcpy(&buffer[82], "FAT32   ", 8);
        fs_log_info("fs type is %s\nstart format...", "FAT32");
    }
#endif /* FS_FAT_NOFAT32==0 */
    /* Signature = 0xAA55 */
    buffer[510] = (char)0x55;
    buffer[511] = (char)0xaa;
    /* Write BPB to media */
    i = format_write_sec((const void *)buffer, 0, 1, pPart);
    if (i < 0)
    {
        pfree(buffer, FSYS_FAT_SEC_SIZE);
        return -1;
    }
    if (FSysType == 2)
    {
        /* Write backup BPB */
        i = format_write_sec((void *)buffer, 6, 1, pPart);
        if (i < 0)
        {
            pfree(buffer, FSYS_FAT_SEC_SIZE);
            return -1;
        }
    }
    /*  Init FAT 1 & 2   */
    memset(buffer, 0x00, FSYS_FAT_SEC_SIZE);
    for (sector = 0; sector < 2 * (bpb.FATSz16 + bpb.FATSz32); sector++)
    {
        value = sector % (bpb.FATSz16 + bpb.FATSz32);
        if (value != 0)
        {
            i = format_write_sec((void *)buffer, bpb.RsvdSecCnt + sector, 1, pPart);
            if (i < 0)
            {
                pfree(buffer, FSYS_FAT_SEC_SIZE);
                return -1;
            }
        }
    }
    buffer[0] = (char)Media;
    buffer[1] = (char)0xff;
    buffer[2] = (char)0xff;
    if (FSysType != 0)
    {
        buffer[3] = (char)0xff;
    }
#if (FSYS_FAT_NOFAT32==0)
    if (FSysType == 2)
    {
        buffer[4]   = (char)0xff;
        buffer[5]   = (char)0xff;
        buffer[6]   = (char)0xff;
        buffer[7]   = (char)0x0f;
        buffer[8]   = (char)0xff;
        buffer[9]   = (char)0xff;
        buffer[10]  = (char)0xff;
        buffer[11]  = (char)0x0f;
    }
#endif /* FS_FAT_NOFAT32==0 */
    for (i = 0; i < 2; i++)
    {
        j = format_write_sec((void *)buffer, (__u32)bpb.RsvdSecCnt + i * ((__u32)bpb.FATSz16 + bpb.FATSz32), 1, pPart);
        if (j < 0)
        {
            pfree(buffer, FSYS_FAT_SEC_SIZE);
            return -1;
        }
    }
    /* Init root directory area */
    memset(buffer, 0x00, (__u32)FSYS_FAT_SEC_SIZE);
    if (bpb.RootEntCnt != 0)
    {
        /* FAT12/FAT16 */
        n = (((__u32)bpb.RootEntCnt * 32) / (__u32)512);
        for (i = 0; i < n; i++)
        {
            j = format_write_sec((void *)buffer, bpb.RsvdSecCnt + 2 * bpb.FATSz16 + i, 1, pPart);
            if (j < 0)
            {
                pfree(buffer, FSYS_FAT_SEC_SIZE);
                return -1;
            }
        }

        /* add vol label FDT */
        if (VolLab)
        {
            strncpy(buffer, VolLab, 11);
            for (i = strlen(VolLab); i < 11; i++)
            {
                buffer[i] = (char)0x20;
            }
            buffer[11] = 0x08;
            if (format_write_sec((void *)buffer,
                                 bpb.RsvdSecCnt + 2 * bpb.FATSz16, 1, pPart) < 0)
            {
                pfree(buffer, FSYS_FAT_SEC_SIZE);
                return -1;
            }
        }
    }
#if (FSYS_FAT_NOFAT32==0)
    else
    {
        /* FAT32 */
        n = bpb.SecPerClus;
        for (i = 0; i < n; i++)
        {
            j = format_write_sec((void *)buffer, bpb.RsvdSecCnt + 2 * bpb.FATSz32 + (bpb.RootClus - 2) * n + i, 1, pPart);
            if (j < 0)
            {
                pfree(buffer, FSYS_FAT_SEC_SIZE);
                return -1;
            }
        }

        /* add vol label FDT */
        if (VolLab)
        {
            strncpy(buffer, VolLab, 11);
            for (i = strlen(VolLab); i < 11; i++)
            {
                buffer[i] = (char)0x20;
            }
            buffer[11] = 0x08;
            if (format_write_sec((void *)buffer, bpb.RsvdSecCnt +
                                 2 * bpb.FATSz32 + (bpb.RootClus - 2) * n, 1, pPart) < 0)
            {
                pfree(buffer, FSYS_FAT_SEC_SIZE);
                return -1;
            }
        }
    }
#endif /* FS_FAT_NOFAT32==0 */
#if (FSYS_FAT_NOFAT32==0)
    if (FSysType == 2)
    {
        /* Init FSInfo */
        memset(buffer, 0x00, FSYS_FAT_SEC_SIZE);
        /* LeadSig = 0x41615252 */
        buffer[0]     = (char)0x52;
        buffer[1]     = (char)0x52;
        buffer[2]     = (char)0x61;
        buffer[3]     = (char)0x41;
        /* StructSig = 0x61417272 */
        buffer[484]   = (char)0x72;
        buffer[485]   = (char)0x72;
        buffer[486]   = (char)0x41;
        buffer[487]   = (char)0x61;
        /* Invalidate last known free cluster count */
        buffer[488]   = (char)0xff;
        buffer[489]   = (char)0xff;
        buffer[490]   = (char)0xff;
        buffer[491]   = (char)0xff;
        /* Give hint for free cluster search */
        buffer[492]   = (char)0xff;
        buffer[493]   = (char)0xff;
        buffer[494]   = (char)0xff;
        buffer[495]   = (char)0xff;
        /* TrailSig = 0xaa550000 */
        buffer[508]   = (char)0x00;
        buffer[509]   = (char)0x00;
        buffer[510]   = (char)0x55;
        buffer[511]   = (char)0xaa;
        i = format_write_sec((void *)buffer, bpb.FSInfo, 1, pPart);
        if (i < 0)
        {
            pfree(buffer, FSYS_FAT_SEC_SIZE);
            return -1;
        }
    }
#endif /* FS_FAT_NOFAT32==0 */
    pfree(buffer, FSYS_FAT_SEC_SIZE);
    return 0;
}

/*
**********************************************************************************************************************
*
*             _FS_FAT_AutoFormat
*
  Description:
  FS internal function. Get information about the media from the
  device driver. Based on that informaton, calculate parameters for
  formatting that media and call the format routine.

  Parameters:
  Idx         - Index of device in the device information table
                referred by fsys_pDevInfo.
  Unit        - Unit number.

  Return value:
  >=0         - Media has been formatted.
  <0          - An error has occured.
**********************************************************************************************************************
*/
int _FS_FAT_AutoFormat(__hdle pPart, __hdle fmtpara)
{
    struct
    {
        __u32 hiddennum;
        __u32 headnum;
        __u32 secnum;
        __u32 partseccnt;
    } devinfo;
    __u32 rootentcnt;
    __u32 fatsz;
    __u32 rootdirsectors;
    __u32 tmpval1;
    __u32 tmpval2;
    __u32 fatsz32;
    __u32 totsec32;
    __u32 resvdseccnt;
    __u16 totsec16;
    __u16 fatsz16;
//    int force_type = 2;
    int force_type = 0xff;
    char *VolLab = NULL;
    int i;
    unsigned char secperclust;
    char fsystype;
    long part_sector_size;

    if (fmtpara)
    {
        VolLab = ((struct fmt_para *)fmtpara)->label;
    }

    /* Get info from device */
    devinfo.hiddennum = 0;
    devinfo.headnum   = 0;
    devinfo.secnum    = 0;
    esFSYS_pioctrl(pPart, PART_IOC_USR_GETPARTSIZE, 0, &part_sector_size);
    devinfo.partseccnt = part_sector_size;

    fs_log_info("part size is %d mbytes\n", (int)(part_sector_size >> 11));
    if (force_type == 0xff)
    {
        if (devinfo.partseccnt <= 1048576UL)
        {
            fsystype = 1;   /* FAT16 */
        }
#if (FSYS_FAT_NOFAT32==0)
        else
        {
            fsystype = 2;   /* FAT32 */
        }
#else
        else
        {
            /* FAT32 disabled */
            return -1;
        }
#endif /* FS_FAT_NOFAT32==0 */
    }
    else if (force_type == 1 || force_type == 2)
    {
        fsystype = force_type;
    }
    else
    {
        return -1;
    }

    /* Set correct RootEntCnt & ResvdSecCnt */
    if (fsystype != 2)
    {
        /* FAT16 */
        i = 0;
        while (_FS_auto_rootcnt[i].SecNum < devinfo.partseccnt)
        {
            i++;
        }
        rootentcnt = _FS_auto_rootcnt[i].Num;
        resvdseccnt = 1;
    }
#if (FSYS_FAT_NOFAT32==0)
    else
    {
        /* FAT32 */
        rootentcnt = 0;
        resvdseccnt = 0x20;
    }
#endif /* FS_FAT_NOFAT32==0 */

    /* Determinate SecPerClust */
    i = 0;
    while (_FS_auto_secperclust[i].SecNum < devinfo.partseccnt)
    {
        i++;
    }
    secperclust = (unsigned char) _FS_auto_secperclust[i].Num;
    /*
        FAT16/FAT32 FAT size calculation
        The formula used is following the Microsoft specification
        version 1.03 very closely. Therefore the calculated FAT
        size can be up to 8 sectors bigger than required for the
        media. This is a waste of up to 8 sectors, but not a problem
        regarding correctness of the media's data.
    */
    rootdirsectors = ((rootentcnt * 32) + 511) / 512;
    tmpval1 = devinfo.partseccnt - (resvdseccnt + rootdirsectors);
    tmpval2 = (256 * secperclust) + 2;
#if (FSYS_FAT_NOFAT32==0)
    if (fsystype == 2)
    {
        tmpval2 = tmpval2 / 2;
    }
#endif /* FS_FAT_NOFAT32==0 */
    fatsz = (tmpval1 + (tmpval2 - 1)) / tmpval2;
    if (fsystype != 2)
    {
        fatsz16 = fatsz;
        fatsz32 = 0;
    }
#if (FSYS_FAT_NOFAT32==0)
    else
    {
        fatsz16 = 0;
        fatsz32 = fatsz;
    }
#endif /* FS_FAT_NOFAT32==0 */

    /* Number of total sectors (512 byte units) of the media
         This is independent of FAT type (FAT12/FAT16/FAT32) */
    if (devinfo.partseccnt < 0x10000UL)
    {
        totsec16 = devinfo.partseccnt;
        totsec32 = 0;
    }
    else
    {
        totsec16 = 0;
        totsec32 = devinfo.partseccnt;
    }
    /* Format media using calculated values */
    i = _fsys_fat_format(pPart,
                         secperclust,
                         (__u16)rootentcnt,
                         totsec16,
                         totsec32,
                         (char)0xf8,
                         fatsz16,
                         fatsz32,
                         (__u16)devinfo.secnum,
                         (__u16)devinfo.headnum,
                         devinfo.hiddennum,
                         fsystype,
                         VolLab);
    return i;
}
#endif  /* FSYS_FAT_FORMAT_EN */