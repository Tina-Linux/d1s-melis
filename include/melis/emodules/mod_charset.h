#ifndef __MOD_CHARSET__
#define __MOD_CHARSET__
#include <typedef.h>

typedef  enum
{
    EPDK_CHARSET_ENM_GB2312  = 0,       // 简体中文
    EPDK_CHARSET_ENM_UTF8,              // utf8
    EPDK_CHARSET_ENM_UTF16BE,           // utf16be
    EPDK_CHARSET_ENM_UTF16LE,           // utf16le
    EPDK_CHARSET_ENM_BIG5,              // 繁体中文
    EPDK_CHARSET_ENM_GBK,               // 中文
    EPDK_CHARSET_ENM_SJIS,              // 日文
    EPDK_CHARSET_ENM_EUC_JP,            // 日文, 暂不支持
    EPDK_CHARSET_ENM_EUC_KR,            // 韩文
    EPDK_CHARSET_ENM_KIO8_R,            // 俄文
    EPDK_CHARSET_ENM_ISO_8859_1,        // cp1252.西欧语言
    EPDK_CHARSET_ENM_ISO_8859_2,        // 中欧语言
    EPDK_CHARSET_ENM_ISO_8859_3,        // 南欧语言
    EPDK_CHARSET_ENM_ISO_8859_4,        // 北欧语言
    EPDK_CHARSET_ENM_ISO_8859_5,        // 西里尔字母
    EPDK_CHARSET_ENM_ISO_8859_6,        // 阿拉伯语
    EPDK_CHARSET_ENM_ISO_8859_7,        // 希腊语
    EPDK_CHARSET_ENM_ISO_8859_8,        // 希伯来语, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_9,        // cp1254，土耳其语
    EPDK_CHARSET_ENM_ISO_8859_10,       // 北欧斯堪的纳维亚语系
    EPDK_CHARSET_ENM_ISO_8859_11,       // 泰语, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_12,       // 梵文, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_13,       // 波罗的海语系
    EPDK_CHARSET_ENM_ISO_8859_14,       // 凯尔特人语系
    EPDK_CHARSET_ENM_ISO_8859_15,       // 扩展了法语和芬兰语的西欧语系
    EPDK_CHARSET_ENM_ISO_8859_16,       // 扩展的东南欧语系, 暂不支持
    EPDK_CHARSET_ENM_CP874,             // 泰文
    EPDK_CHARSET_ENM_CP1250,            // 中欧
    EPDK_CHARSET_ENM_CP1251,            // 西里尔文
    EPDK_CHARSET_ENM_CP1253,            // 希腊文, 暂不支持
    EPDK_CHARSET_ENM_CP1255,            // 希伯来文
    EPDK_CHARSET_ENM_CP1256,            // 阿拉伯文, 暂不支持
    EPDK_CHARSET_ENM_CP1257,            // 波罗的海文, 暂不支持
    EPDK_CHARSET_ENM_CP1258,            // 越南, 暂不支持
    EPDK_CHARSET_ENM_CP1252,            // 西欧语言。cp1252是iso-8859-1的超集
    EPDK_CHARSET_ENM_CP1254,            // 土耳其语。cp1254是iso-8859-9的超集
    EPDK_CHARSET_ENM_JIS,               // 日文JIS字符编码,暂不支持
    EPDK_CHARSET_ENM_NUM,

    EPDK_CHARSET_ENM_UNKNOWN = -1       // always the last one
} __epdk_charset_enm_e;


#define EPDK_CHARSET_NAME_GB2312        "CHARSET_GB2312"
#define EPDK_CHARSET_NAME_UTF8          "CHARSET_UTF8"
#define EPDK_CHARSET_NAME_UTF16BE       "CHARSET_UTF16BE"
#define EPDK_CHARSET_NAME_UTF16LE       "CHARSET_UTF16LE"
#define EPDK_CHARSET_NAME_BIG5          "CHARSET_BIG5"
#define EPDK_CHARSET_NAME_GBK           "CHARSET_GBK"
#define EPDK_CHARSET_NAME_SJIS          "CHARSET_SJIS"
#define EPDK_CHARSET_NAME_EUC_JP        "CHARSET_EUC_JP"
#define EPDK_CHARSET_NAME_EUC_KR        "CHARSET_EUC_KR"
#define EPDK_CHARSET_NAME_KIO8_R        "CHARSET_KIO8_R"
#define EPDK_CHARSET_NAME_ISO_8859_1    "CHARSET_ISO_8859_1"
#define EPDK_CHARSET_NAME_ISO_8859_2    "CHARSET_ISO_8859_2"
#define EPDK_CHARSET_NAME_ISO_8859_3    "CHARSET_ISO_8859_3"
#define EPDK_CHARSET_NAME_ISO_8859_4    "CHARSET_ISO_8859_4"
#define EPDK_CHARSET_NAME_ISO_8859_5    "CHARSET_ISO_8859_5"
#define EPDK_CHARSET_NAME_ISO_8859_6    "CHARSET_ISO_8859_6"
#define EPDK_CHARSET_NAME_ISO_8859_7    "CHARSET_ISO_8859_7"
#define EPDK_CHARSET_NAME_ISO_8859_8    "CHARSET_ISO_8859_8"
#define EPDK_CHARSET_NAME_ISO_8859_9    "CHARSET_ISO_8859_9"
#define EPDK_CHARSET_NAME_ISO_8859_10   "CHARSET_ISO_8859_10"
#define EPDK_CHARSET_NAME_ISO_8859_11   "CHARSET_ISO_8859_11"
#define EPDK_CHARSET_NAME_ISO_8859_12   "CHARSET_ISO_8859_12"
#define EPDK_CHARSET_NAME_ISO_8859_13   "CHARSET_ISO_8859_13"
#define EPDK_CHARSET_NAME_ISO_8859_14   "CHARSET_ISO_8859_14"
#define EPDK_CHARSET_NAME_ISO_8859_15   "CHARSET_ISO_8859_15"
#define EPDK_CHARSET_NAME_ISO_8859_16   "CHARSET_ISO_8859_16"
#define EPDK_CHARSET_NAME_CP874         "CHARSET_CP874"
#define EPDK_CHARSET_NAME_CP1250        "CHARSET_CP1250"
#define EPDK_CHARSET_NAME_CP1251        "CHARSET_CP1251"
#define EPDK_CHARSET_NAME_CP1253        "CHARSET_CP1253"
#define EPDK_CHARSET_NAME_CP1255        "CHARSET_CP1255"
#define EPDK_CHARSET_NAME_CP1256        "CHARSET_CP1256"
#define EPDK_CHARSET_NAME_CP1257        "CHARSET_CP1257"
#define EPDK_CHARSET_NAME_CP1258        "CHARSET_CP1258"


#define EPDK_CHARSET_NAME_LEN_MAX       31

typedef struct CHARSET_API_ENTRY
{
    __s32(*ptr_CHS_Uni2Char)(__s32 charset_type, __u16 uni, __u8 *str, __u32 len);
    __s32(*ptr_CHS_Char2Uni)(__s32 charset_type, const __u8 *str, __u32 len, __u16 *uni);
    __u32(*ptr_CHS_GetChUpperTbl)(__s32 charset_type, void *buf, __u32 size);
    __u32(*ptr_CHS_GetChLowerTbl)(__s32 charset_type, void *buf, __u32 size);
} charset_api_entry_t;



#ifndef USED_BY_CHARSET //not used by charset module itsef
#define CHARSET_ENTRY_TBL   (0xe4400000)   //this address is depended on the link address of mod_charset
#define CHARSET_FUNC_ENTRY  ((charset_api_entry_t *)CHARSET_ENTRY_TBL)
#define CHS_Uni2Char        (CHARSET_FUNC_ENTRY->ptr_CHS_Uni2Char   )
#define CHS_Char2Uni        (CHARSET_FUNC_ENTRY->ptr_CHS_Char2Uni   )
#define CHS_GetChUpperTbl   (CHARSET_FUNC_ENTRY->ptr_CHS_GetChUpperTbl  )
#define CHS_GetChLowerTbl   (CHARSET_FUNC_ENTRY->ptr_CHS_GetChLowerTbl  )
#endif

#endif
