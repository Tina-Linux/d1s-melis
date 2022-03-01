#ifndef _MTPTYPES_H
#define _MTPTYPES_H
#include "stdint.h"

//typedef unsigned int uint32_t;
//#ifndef size_t
//typedef unsigned int size_t;
//#endif
//typedef unsigned short uint16_t;

typedef uint32_t uint128_t[4];

typedef uint16_t MtpOperationCode;
typedef uint16_t MtpResponseCode;
typedef uint16_t MtpEventCode;
typedef uint32_t MtpSessionID;
typedef uint32_t MtpStorageID;
typedef uint32_t MtpTransactionID;
typedef uint16_t MtpPropertyCode;
typedef uint16_t MtpDataType;
typedef uint16_t MtpObjectFormat;
typedef MtpPropertyCode MtpDeviceProperty;
typedef MtpPropertyCode MtpObjectProperty;

typedef uint32_t MtpObjectHandle;

typedef struct
{
    uint16_t *array;
    unsigned int size;
} UInt16List;

typedef UInt16List MtpDevicePropertyList;
typedef UInt16List MtpObjectFormatList;




#define MTP_PARENT_ROOT         0xFFFFFFFF       // parent is root of the storage
#define kInvalidObjectHandle    0xFFFFFFFF

#endif
