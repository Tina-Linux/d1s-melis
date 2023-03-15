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
#include "spsc_ctrl.h"
#include "spsc_ctrl_ui.h"

static movie_spsc_ctrl_uipara_t uipara_480_272 =
{
    {
        0, 0, 480, 70,
        {0,0},
        {0,0},
        0
    },
    1,
    //movie_spsc_ctrl_icon_playpause,
    {
        0, 0, 480, 70,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0},
        0
    },
    {
        {
            236, 44, 19, 21,
            {
                ID_MOVIE_SPSC_PREV_UF_BMP,
                ID_MOVIE_SPSC_PREV_F_BMP
            },
            {0,0},
            0
        },
        {
            258, 44, 19, 21,
            {
                ID_MOVIE_SPSC_NEXT_UF_BMP,
                ID_MOVIE_SPSC_NEXT_F_BMP
            },
            {0,0},
            0
        },
        //{
        //  6, 44, 18, 21,
        //  ID_MOVIE_SPSC_PLAY_UF_BMP,
        //  ID_MOVIE_SPSC_PLAY_UF_BMP,
        //}


    },
    {
        {
            453, 47, 24, 16,
            {0,0},
            {0,0},
            0
        },
        {
            299, 47, 19, 16,
            {ID_MOVIE_SPSC_CHANNEL_BMP,0},
            {0,0},
            0
        },
        {
            320, 47, 24, 16,
            {0,0},
            {0,0},
            0
        },
        {
            425, 47, 25, 16,
            {0,0},
            {0,0},
            0
        },
        {
            299, 0, 480 - 299, 23,
            {0,0},
            {0,0},
            0
        },
        {
            299, 24, 480 - 299, 22,
            {0,0},
            {0,0},
            0
        },
        {
            362, 47, 18, 16,
            {ID_MOVIE_SPSC_SUBTITLE_BMP,0},
            {0,0},
            0
        },
        {
            382, 47, 32, 16,
            {0,0},
            {0,0},
            0
        },
        {
            8, 20, 270, 22,
            {0,0},
            {0,0},
            0
        }
    },
    {
        {
            ID_MOVIE_SPSC_ONLY_ONCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ONE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ALL_BMP,0
        },
        {
            ID_MOVIE_SPSC_SEQUENCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_RANDOM_BMP,0
        }
    },
    {
        {
            ID_MOVIE_SPSC_VIDEO_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_SCREEN_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_ORIGIN_SIZE_BMP,0
        },
        {
            ID_MOVIE_SPSC_4R3_BMP,0
        },
        {
            ID_MOVIE_SPSC_16R9_BMP,0
        }
    },
    {
        {
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
        },
        {
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
        }
    },
    {
        181, 46, 55, 16,
        {0,0},
        {0,0},
        0        
    },
    {
        181, 46, 55, 16,
        {0,0},
        {0,0},
        0        
        
    },
    {
        {
            29, 44, 152, 21,
            {ID_MOVIE_PROG_BG_BMP,0},
            {0,0},
            0
        },
        {
            29, 44, 3, 21,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0},
            0
        },
        {
            29, 44, 3, 21,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0},
            0
        },
        {
            29, 44, 13, 21,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0},
            0
        }
    },
};

static movie_spsc_ctrl_uipara_t uipara_800_480 =
{
    {
        0, 0, 800, 56,
        {0,0},
        {0,0},
        0,
    },
    movie_spsc_ctrl_icon_infor,
    {
        0, 0, 800, 56,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0},
        0,
    },
    {

        {
            16, 51 - 46, 58, 48,
            {
                ID_MOVIE_INFOR_BMP,
                ID_MOVIE_INFOR_FCS_BMP
            },
            {0,0},
            0
        },
        {
            647, 47 - 46, 127, 53,
            {
                ID_MOVIE_SPSC_TVOUT_UF_BMP,
                ID_MOVIE_SPSC_TVOUT_F_BMP
            },
            {0,0},
            0            
        },
    },
    {
        {
            453, 47, 24, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            299, 47, 19, 16,
            {ID_MOVIE_SPSC_CHANNEL_BMP,0},
            {0,0},
            0            
        },
        {
            320, 47, 24, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            425, 47, 25, 16,
            {0,0},
            {0,0},
            0           
        },
        {
            299, 0, 480 - 299, 23,
            {0,0},
            {0,0},
            0            
        },
        {
            299, 24, 480 - 299, 22,
            {0,0},
            {0,0},
            0
        },
        {
            362, 47, 18, 16,
            {ID_MOVIE_SPSC_SUBTITLE_BMP,0},
            {0,0},
            0            
        },
        {
            382, 47, 32, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            8, 20, 270, 22,
            {0,0},
            {0,0},
            0
        }
    },
    {
        {
            ID_MOVIE_SPSC_ONLY_ONCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ONE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ALL_BMP,0
        },
        {
            ID_MOVIE_SPSC_SEQUENCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_RANDOM_BMP,0
        }
    },
    {
        {
            ID_MOVIE_SPSC_VIDEO_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_SCREEN_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_ORIGIN_SIZE_BMP,0
        },
        {
            ID_MOVIE_SPSC_4R3_BMP,0
        },
        {
            ID_MOVIE_SPSC_16R9_BMP,0
        }
    },
    {
        {
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
        },
        {
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
        },
    },
    {
        100, 30, 80, 20,
        {0,0},
        {0,0},
        0        
    },
    {
        600 - 30 - 10, 30, 80, 20,
        {0,0},
        {0,0},
        0
    },
    {
        {
            79 + 50, 65 - 46 - 10, 457, 20,
            {ID_MOVIE_SPSC_PROG_BG_BMP,0},
            {0,0},
            0            
            //79, 65-46-10, 680, 26,
            //ID_INIT_PROG_BG_BMP
        },
        {
            79 + 50, 65 - 46 - 10, 12, 20,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0},
            0
            //79, 65-46-10, 12, 26,
            //ID_INIT_PROG_LEFT_BMP
        },
        {
            79 + 50, 65 - 46 - 10, 12, 20,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0},
            0
            // 79, 65-46-10, 12, 26,
            //ID_INIT_PROG_MID_BMP
        },
        {
            79 + 50, 65 - 46 - 10, 24, 20,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0},
            0
            //79, 65-46-10, 26, 26,
            //ID_INIT_PROG_CURSOR_BMP
        },
    },
};


static movie_spsc_ctrl_uipara_t uipara_720_480 =
{
    {
        0, 0, 720, 56,
        {0,0},
        {0,0},
        0        
    },
    movie_spsc_ctrl_icon_infor,
    {
        0, 0, 720, 56,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0},
        0
    },
    {

        {
            16, 51 - 46, 58, 48,
            {
                ID_MOVIE_INFOR_BMP,
                ID_MOVIE_INFOR_FCS_BMP
            },
            {0,0},
            0            
        },
        {
            647 - 60, 47 - 46, 127, 53,
            {
                ID_MOVIE_SPSC_TVOUT_UF_BMP,
                ID_MOVIE_SPSC_TVOUT_F_BMP
            },
            {0,0},
            0            
        },
    },
    {
        //无用
        {
            453, 47, 24, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            299, 47, 19, 16,
            {ID_MOVIE_SPSC_CHANNEL_BMP,0},
            {0,0},
            0            
        },
        {
            320, 47, 24, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            425, 47, 25, 16,
            {0,0},
            {0,0},
            0            
        },
        {
            299, 0, 480 - 299, 23,
            {0,0},
            {0,0},
            0            
        },
        {
            299, 24, 480 - 299, 22,
            {0,0},
            {0,0},
            0            
        },
        {
            362, 47, 18, 16,
            {ID_MOVIE_SPSC_SUBTITLE_BMP,0},
            {0,0},
            0            
        },
        {
            382, 47, 32, 16,
            {0,0},
            {0,0},
            0
        },
        {
            8, 20, 270, 22,
            {0,0},
            {0,0},
            0
        }
    },
    {
        {
            ID_MOVIE_SPSC_ONLY_ONCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ONE_BMP,0
        },
        {
            ID_MOVIE_SPSC_ROTATE_ALL_BMP,0
        },
        {
            ID_MOVIE_SPSC_SEQUENCE_BMP,0
        },
        {
            ID_MOVIE_SPSC_RANDOM_BMP,0
        }
    },
    {
        {
            ID_MOVIE_SPSC_VIDEO_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_SCREEN_RATIO_BMP,0
        },
        {
            ID_MOVIE_SPSC_ORIGIN_SIZE_BMP,0
        },
        {
            ID_MOVIE_SPSC_4R3_BMP,0
        },
        {
            ID_MOVIE_SPSC_16R9_BMP,0
        }
    },
    {
        {
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PLAY_UF_BMP,0
            },
        },
        {
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
            {
                ID_MOVIE_SPSC_PAUSE_UF_BMP,0
            },
        },
    },
    {
        100, 30, 80, 20,
        {0,0},
        {0,0},
        0        
    },
    {
        600 - 30 - 70, 30, 80, 20,
        {0,0},
        {0,0},
        0
    },
    {
        {
            79, 65 - 46 - 10, 457, 20,
            {ID_MOVIE_SPSC_PROG_BG_BMP,0},
            {0,0},
            0
            //79, 65-46-10, 680, 26,
            //ID_INIT_PROG_BG_BMP
        },
        {
            79, 65 - 46 - 10, 12, 20,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0},
            0
            //79, 65-46-10, 12, 26,
            //ID_INIT_PROG_LEFT_BMP
        },
        {
            79, 65 - 46 - 10, 12, 20,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0},
            0
            // 79, 65-46-10, 12, 26,
            //ID_INIT_PROG_MID_BMP
        },
        {
            79, 65 - 46 - 10, 24, 20,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0},
            0            
            //79, 65-46-10, 26, 26,
            //ID_INIT_PROG_CURSOR_BMP
        },
    },
};


movie_spsc_ctrl_uipara_t *movie_spsc_ctrl_get_uipara(__s32 rotate)
{
    __s32           screen_width;
    __s32           screen_height;
    /* get lcd size*/
    //  __msg("++++++%s, %d+++++++\n", __FILE__, __LINE__);
    dsk_display_get_size(&screen_width, &screen_height);

    switch (rotate)
    {
        case GUI_SCNDIR_NORMAL:
        case GUI_SCNDIR_ROTATE180:
        {
            if ((screen_width == 480) && (screen_height == 272))
            {
                return &uipara_480_272;
            }
            else if ((screen_width == 800) && (screen_height == 480))
            {
                return &uipara_800_480;
            }
            else if ((screen_width == 720) && (screen_height == 480 || screen_height == 576))
            {
                return &uipara_720_480;
            }
        }

        case GUI_SCNDIR_ROTATE90:
        case GUI_SCNDIR_ROTATE270:
            break;
    }

    return &uipara_800_480;
}
