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
#include <typedef.h>
#include <libc/mediainfo/eLIBs_GetAudioFmt.h>
#include <libc.h>

extern int GetAudioFormat(const char *file, int *format);
extern int GetAudioInfo(const char *file, __audio_file_info_t *inf);
extern __bool GetAudioDataInfo(const char *file, __audio_file_info_t *inf, __s8 *buf, __s32 datalen);
extern int setAudioEncrytpMode(int mode);


typedef struct __MEDIA_FMT_TBL_IDX
{
    char  suffix[8];
    int   format;
} __media_fmt_tbl_idx_t;


//video file format table, need end of "NULL"
static __media_fmt_tbl_idx_t video_fmt_tbl[] =
{
    { ".AVI",  SW_VIDEO_FORMAT_AVI      },
    { ".DIVX", SW_VIDEO_FORMAT_AVI      },
    { ".PMP",  SW_VIDEO_FORMAT_PMP      },
    { ".MOV",  SW_VIDEO_FORMAT_MOV      },
    { ".3GP",  SW_VIDEO_FORMAT_3GP      },
    { ".MP4",  SW_VIDEO_FORMAT_MP4      },
    { ".M4V",  SW_VIDEO_FORMAT_MP4      },
    { ".F4V",  SW_VIDEO_FORMAT_FLV      },
    { ".MPG",  SW_VIDEO_FORMAT_MPG      },
    { ".MPEG", SW_VIDEO_FORMAT_MPG      },
    { ".VOB",  SW_VIDEO_FORMAT_VOB      },
    { ".DAT",  SW_VIDEO_FORMAT_DAT      },
    { ".RM",   SW_VIDEO_FORMAT_RM       },
    { ".RAM",  SW_VIDEO_FORMAT_RM       },
    { ".RMVB", SW_VIDEO_FORMAT_RMVB     },
    { ".FLV", SW_VIDEO_FORMAT_FLV      },
    { ".WMV", SW_VIDEO_FORMAT_WMV      },
    { ".ASF", SW_VIDEO_FORMAT_ASF      },
    { ".MKV", SW_VIDEO_FORMAT_MKV      },
    { ".PSR", SW_VIDEO_FORMAT_PSR      },
    { ".TP", SW_VIDEO_FORMAT_TP       },
    { ".TRP", SW_VIDEO_FORMAT_TP       },
    { ".TS", SW_VIDEO_FORMAT_TS       },
    { ".M2TS", SW_VIDEO_FORMAT_TS       },
    { ".MTS",  SW_VIDEO_FORMAT_TS       },
    { "NULL",  SW_VIDEO_FORMAT_UNKNOWN  }
};

//picture file format, need end of "NULL"
static __media_fmt_tbl_idx_t pic_fmt_tbl[] =
{
    { ".BMP",  SW_PIC_FORMAT_BMP        },
    { ".GIF",  SW_PIC_FORMAT_GIF        },
    { ".JPG",  SW_PIC_FORMAT_JPG        },
    { ".JPEG", SW_PIC_FORMAT_JPG        },
    { ".JFIF", SW_PIC_FORMAT_JPG        },
    { ".PNG",  SW_PIC_FORMAT_PNG        },
    { "NULL",  SW_PIC_FORMAT_UNKNOWN    }
};

//ebook file format, need end of "NULL"
static __media_fmt_tbl_idx_t ebook_fmt_tbl[] =
{
    { ".TXT",  SW_EBOOK_FORMAT_TXT      },
    { ".H",    SW_EBOOK_FORMAT_H        },
    { ".C",    SW_EBOOK_FORMAT_C        },
    { ".INI",  SW_EBOOK_FORMAT_INI      },
    { ".LRC",  SW_EBOOK_FORMAT_LRC      },
    { ".SRT",  SW_EBOOK_FORMAT_SRT      },
    { ".SSA",  SW_EBOOK_FORMAT_TXT      },
    { ".ASS",  SW_EBOOK_FORMAT_TXT      },
    { ".PDF",  SW_EBOOK_FORMAT_PDF      },

    { ".EPUB", SW_EBOOK_FORMAT_EPUB     },
    { ".FB2",  SW_EBOOK_FORMAT_FB2      },
    { ".PDB",  SW_EBOOK_FORMAT_PDB      },
    { ".RTF",  SW_EBOOK_FORMAT_RTF      },
    { ".DOC",  SW_EBOOK_FORMAT_DOC      },
    { ".HTML", SW_EBOOK_FORMAT_HTML     },
    { ".MOBI", SW_EBOOK_FORMAT_MOBI     },

    { ".CHM",   SW_EBOOK_FORMAT_CHM     },
    { ".DJVU",  SW_EBOOK_FORMAT_DJVU    },
    { ".TCR",   SW_EBOOK_FORMAT_TCR     },
    { ".HTM",   SW_EBOOK_FORMAT_HTM     },
    { ".OEB",   SW_EBOOK_FORMAT_OEB     },
    { ".IW44",  SW_EBOOK_FORMAT_IW44    },
    { ".IW4",   SW_EBOOK_FORMAT_IW4     },
    { ".DJV",   SW_EBOOK_FORMAT_DJV     },
    { ".PRC",   SW_EBOOK_FORMAT_PRC     },
    { ".OPF",   SW_EBOOK_FORMAT_OPF     },

    { "NULL",   SW_EBOOK_FORMAT_UNKNOWN  }
};

//game file format, need end of "NULL"
static __media_fmt_tbl_idx_t game_fmt_tbl[] =
{
    { ".GAME",  SW_GAME_FORMAT_EPOS      },
    { ".UXE",   SW_GAME_FORMAT_EPOS      },
    { "NULL",   SW_GAME_FORMAT_UNKNOWN   }
};


//flash file format, need end of "NULL"
static __media_fmt_tbl_idx_t flash_fmt_tbl[] =
{
    { ".SWF",   SW_FLASH_FORMAT_SWF      },
    { "NULL",   SW_GAME_FORMAT_UNKNOWN   }
};

/*
************************************************************************************************************************
*                       PARSE MEDIA FILE FORMAT
*
*Description: parse media file format.
*
*Arguments  : table     media format table.
*             file      full path of video file.
*             format    file format information.
*
*Return     : result.
*               1:get video file format succssed.
*               0:get video file format failed.
************************************************************************************************************************
*/
static __bool ParseMediaFormat(__media_fmt_tbl_idx_t *table, const char *file, int *format)
{
    __s32   i;
    char    *suffix;

    //check if pointer is valid
    if (!table || !file || !format)
    {
        return 0;
    }

    //seek to extended name
    suffix = eLIBs_strchrlast((char *)file, '.');

    //look for the suffix in media format table
    for (i = 0; eLIBs_strnicmp(table[i].suffix, "NULL", 4) != 0; i++)
    {
        if (eLIBs_stricmp(suffix, table[i].suffix) == 0)
        {
            *format = table[i].format;
            return 1;
        }
    }

    *format = table[i].format;
    return 0;
}


/*
************************************************************************************************************************
*                       GET AUDIO FORMAT
*
*Description: get audio format.
*
*Arguments  : file      full path of audio file.
*             format    file format information.
*
*Return     : result.
*               1:get audio format succssed.
*               0:get audio format failed.
************************************************************************************************************************
*/
__bool eLIBs_GetAudioFormat(const char *file, int *format)
{
    return (__bool)GetAudioFormat(file, format);
}


/*
************************************************************************************************************************
*                       GET AUDIO FILE INFORMATION
*
*Description: get audio file information.
*
*Arguments  : file      full path of audio file.
*             inf       file information.
*
*Return     : result.
*               1:get audio information succssed.
*               0:get audio information failed.
************************************************************************************************************************
*/
__bool eLIBs_GetAudioInfo(const char *file, __audio_file_info_t *inf)
{
    return (__bool)GetAudioInfo(file, inf);
}

/*
************************************************************************************************************************
*                       GET AUDIO FILE INFORMATION
*
*Description: get audio file information.
*
*Arguments  : file      full path of audio file.
*             inf       file information.
*
*Return     : result.
*               1:get audio information succssed.
*               0:get audio information failed.
************************************************************************************************************************
*/
__bool eLIBs_GetAudioDataInfo(const char *pFilename, __audio_file_info_t *AIF, __s8 *buf, __s32 datalen)
{
    return GetAudioDataInfo(pFilename, AIF, buf, datalen);
}



/*
************************************************************************************************************************
*                       GET VIDEO FILE FORMAT
*
*Description: get video file format.
*
*Arguments  : file      full path of video file.
*             format    file format information.
*
*Return     : result.
*               1:get video file format succssed.
*               0:get video file format failed.
************************************************************************************************************************
*/
__bool eLIBs_GetVideoFormat(const char *file, int *format)
{
    return ParseMediaFormat(video_fmt_tbl, file, format);
}


/*
************************************************************************************************************************
*                       GET PICTURE FORMAT
*
*Description: get picture format.
*
*Arguments  : file      full path of picture file.
*             format    file format information.
*
*Return     : result.
*               1:get picture format succssed.
*               0:get picture format failed.
************************************************************************************************************************
*/
__bool eLIBs_GetPicFormat(const char *file, int *format)
{
    return ParseMediaFormat(pic_fmt_tbl, file, format);
}


/*
************************************************************************************************************************
*                       GET EBOOK FORMAT
*
*Description: get ebook format.
*
*Arguments  : file      full path of ebook file.
*             format    file format information.
*
*Return     : result.
*               1:get ebook format succssed.
*               0:get ebook format failed.
************************************************************************************************************************
*/
__bool eLIBs_GetEbookFormat(const char *file, int *format)
{
    return ParseMediaFormat(ebook_fmt_tbl, file, format);
}


/*
************************************************************************************************************************
*                       GET GAME FILE FORMAT
*
*Description: get game file format.
*
*Arguments  : file      full path of game file.
*             format    file format information.
*
*Return     : result.
*               1:get game file format succssed.
*               0:get game file format failed.
************************************************************************************************************************
*/
__bool eLIBs_GetGameFormat(const char *file, int *format)
{
    return ParseMediaFormat(game_fmt_tbl, file, format);
}


/*
*********************************************************************************************************
*                        GET FLASH FILE FORMAT
*
* Description: get flash file format.
*
*Arguments  : file      full path of flash file.
*             format    file format information.
*
*Return     : result.
*               1:get flash file format succssed.
*               0:get flash file format failed.
*********************************************************************************************************
*/
__bool eLIBs_GetFlashFormat(const char *file, int *format)
{
    return ParseMediaFormat(flash_fmt_tbl, file, format);
}


int eLIBs_setAudioEncrytpMode(int mode)
{
    return setAudioEncrytpMode(mode);
}
