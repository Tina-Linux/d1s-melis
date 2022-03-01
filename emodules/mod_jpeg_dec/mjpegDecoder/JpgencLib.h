#ifndef __JPGENCLIB_H
#define __JPGENCLIB_H

#define MAX_FRM_NUM         5

#define OUT_STRM_BUFFER_LEN (6784*1024)//(6784*1024)
#define MAX_BUF_LEN         (15*1024*1024)
#define MIN_BUF_LEN         (6784*1024)//(6784*1024)

#define MAX_ONE_FRAME_LEN   (256*1024)
#define MAX_BS_FRAME_IN_VBV 180     // for fps = 30, than we can store data time = 180/30 = 6s, I think it is bigger enough

#define MAX_LIST_SIZE       8
#define MAX_REFERENCE_PICTURES  32               //!< H.264 allows 32 fields

#define MAXSPS  1
#define MAXPPS  1

//#define NUM_QUANT_TBLS 2
#define DCTSIZE         8   /* The basic DCT block is 8x8 samples */
#define DCTSIZE2        64  /* DCTSIZE squared; # of elements in a block */

#define EXIF
#define EXIF_ELEMENT_LEN 12

#define EXIF_BUF_LEN 64
#define EXIF_SUB_IFD0_ADDR  0x200  //in IFD0 for the SUB_IFD0 OFFSET ADDR
#define THUMB_WIDTH  320
#define THUMB_HEIGHT 240

#define EXIF_DEF_FOCAL_LEN_DEN      100

//for exif
#define FMT_BYTE       1
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12


#define TAG_COMPRESSION        0x0103
#define TAG_IMAGEWIDTH         0x0100
#define TAG_IMAGEHEIGHT        0x0101

#define TAG_DESCRIPTION        0x010E
#define TAG_MAKE              0x010F
#define TAG_MODEL             0x0110

#define TAG_ORIENTATION       0x0112
#define TAG_XRESOLUTION       0x011A
#define TAG_YRESOLUTION       0x011B
#define TAG_RESOLUTIONUNIT    0x0128
#define TAG_SOFTWARE           0x0131
#define TAG_DATETIME           0x0132
#define TAG_THUMBNAIL_OFFSET   0x0201
#define TAG_THUMBNAIL_LENGTH   0x0202
#define TAG_Y_CB_CR_POS        0x0213           // Not processed. Format int16u. Values: 1-Centered; 2-Co-sited
#define TAG_EXPOSURETIME       0x829A
#define TAG_FNUMBER            0x829D
#define TAG_EXIF_OFFSET        0x8769
#define TAG_EXPOSURE_PROGRAM   0x8822
#define TAG_GPSINFO            0x8825
#define TAG_ISO_EQUIVALENT     0x8827
#define TAG_EXIF_VERSION       0x9000           // Not processed.
#define TAG_COMPONENT_CFG      0x9101           // Not processed.
#define TAG_DATETIME_ORIGINAL  0x9003
#define TAG_DATETIME_DIGITIZED 0x9004
#define TAG_SHUTTERSPEED       0x9201
#define TAG_APERTURE           0x9202
#define TAG_EXPOSURE_BIAS      0x9204
#define TAG_MAXAPERTURE        0x9205
#define TAG_SUBJECT_DISTANCE   0x9206
#define TAG_METERING_MODE      0x9207
#define TAG_LIGHT_SOURCE       0x9208
#define TAG_FLASH              0x9209
#define TAG_FOCALLENGTH        0x920A
#define TAG_MAKER_NOTE         0x927C           // Not processed yet. Maybe in the future.
#define TAG_USERCOMMENT        0x9286
#define TAG_FLASHPIX_VERSION   0xA000           // Not processed.
#define TAG_COLOUR_SPACE       0xA001           // Not processed. Format int16u. Values: 1-RGB; 2-Adobe RGB 65535-Uncalibrated
#define TAG_EXIF_IMAGEWIDTH    0xa002
#define TAG_EXIF_IMAGELENGTH   0xa003
#define TAG_INTEROP_OFFSET     0xa005
#define TAG_FOCALPLANEXRES     0xa20E
#define TAG_FOCALPLANEUNITS    0xa210
#define TAG_EXPOSURE_INDEX     0xa215
#define TAG_EXPOSURE_MODE      0xa402
#define TAG_WHITE_BALANCE      0xA403
#define TAG_DIGITALZOOMRATIO   0xA404
#define TAG_FOCALLENGTH_35MM   0xa405

#define TAG_GPS_LAT_REF        0x1      //GPSLatitudeRef
#define TAG_GPS_LAT            0x2      //GPSLatitude
#define TAG_GPS_LONG_REF       0x3      //GPSLongitudeRef
#define TAG_GPS_LONG           0x4      //GPSLongitude
#define TAG_GPS_ALT_REF        0x5      //GPSAltitudeRef
#define TAG_GPS_ALT            0x6     //GPSAltitude
#define TAG_GPS_TIME_STAMP     0x7     //GPSTimeStamp
#define TAG_GPS_DATA_STAMP     0x001D   //GPSDateStamp
#define TAG_GPS_PROCESS_METHOD 0x1B    //GPSProcessingMethod


typedef enum
{
    TOP_LEFT = 0x1,
    LEFT_TOP = 0x6,
    LEFT_BOTTOM = 0x8,
    BOTTOM_LEFT = 0x3
} JPEG_ORIEN;


typedef enum            /* JPEG marker codes */
{
    M_SOF0  = 0xc0,
    M_SOF1  = 0xc1,
    M_SOF2  = 0xc2,
    M_SOF3  = 0xc3,

    M_SOF5  = 0xc5,
    M_SOF6  = 0xc6,
    M_SOF7  = 0xc7,

    M_JPG   = 0xc8,
    M_SOF9  = 0xc9,
    M_SOF10 = 0xca,
    M_SOF11 = 0xcb,

    M_SOF13 = 0xcd,
    M_SOF14 = 0xce,
    M_SOF15 = 0xcf,

    M_DHT   = 0xc4,

    M_DAC   = 0xcc,

    M_RST0  = 0xd0,
    M_RST1  = 0xd1,
    M_RST2  = 0xd2,
    M_RST3  = 0xd3,
    M_RST4  = 0xd4,
    M_RST5  = 0xd5,
    M_RST6  = 0xd6,
    M_RST7  = 0xd7,

    M_SOI   = 0xd8,
    M_EOI   = 0xd9,
    M_SOS   = 0xda,
    M_DQT   = 0xdb,
    M_DNL   = 0xdc,
    M_DRI   = 0xdd,
    M_DHP   = 0xde,
    M_EXP   = 0xdf,

    M_APP0  = 0xe0,
    M_APP1  = 0xe1,
    M_APP2  = 0xe2,
    M_APP3  = 0xe3,
    M_APP4  = 0xe4,
    M_APP5  = 0xe5,
    M_APP6  = 0xe6,
    M_APP7  = 0xe7,
    M_APP8  = 0xe8,
    M_APP9  = 0xe9,
    M_APP10 = 0xea,
    M_APP11 = 0xeb,
    M_APP12 = 0xec,
    M_APP13 = 0xed,
    M_APP14 = 0xee,
    M_APP15 = 0xef,

    M_JPG0  = 0xf0,
    M_JPG13 = 0xfd,
    M_COM   = 0xfe,

    M_TEM   = 0x01,

    M_ERROR = 0x100
} JPEG_MARKER;

typedef struct
{
    __u32 num;
    __u32 den;
} rational_t;

typedef struct
{
    __s32 num;
    __s32 den;
} srational_t;


#define SYS_WriteByte(uAddr,bVal) \
    do{*(volatile unsigned char *)(uAddr) = (bVal);}while(0)
#define SYS_WriteWord(uAddr,wVal) \
    do{*(volatile unsigned short *)(uAddr) = (wVal);}while(0)
#define SYS_WriteDWord(uAddr, dwVal) \
			{*(volatile __u32 *)(uAddr) = (dwVal);}


#define SYS_ReadByte(uAddr) \
    (*(volatile unsigned char *)(uAddr))
#define SYS_ReadWord(uAddr) \
    (*(volatile unsigned short *)(uAddr))
#define SYS_ReadDWord(uAddr) \
		(*(volatile __u32 *)(uAddr))


#define PutBits(value,len) \
    writeVeReg(H264ENC_PUTBITSDATA_REG, value);\
    writeVeReg(H264ENC_STARTTRIG_REG, (1<<16)|(len<<8)|PUT_BITS); \
    do {\
        veStatus = readVeReg(H264ENC_STATUS_REG);\
    }while(!(veStatus&&PUTBITS_SATUS))

#endif

