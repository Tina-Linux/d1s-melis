
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_ui.c
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include <log.h>
#include "prog_ui.h"

static movie_prog_uipara_t uipara_480_272 =
{
    1,
    {
        (480 - 296) / 2, 40, 296, 21,
        {0,0},
        {0,0}
    },
    {
        0, 0, 296, 21,
        {0,0},
        {0,0}
    },
    //uipara_bg
    {
        0, 0, 480, 70,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0}
    },
    {
        {
            (296 - 152) / 2, 0, 152, 21,
            {ID_MOVIE_PROG_BG_BMP,0},
            {0,0}
        },
        {
            (296 - 152) / 2, 0, 3, 21,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0}
        },
        {
            (296 - 152) / 2, 0, 3, 21,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0}
            
        },
        {
            (296 - 152) / 2, 0, 13, 21,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0}
        }
    },
    {
        {
            0, 0, 16, 16,
            {0,0},
            {0,0}
            
        },
        {
            24, 0, 16, 16,
            {0,0},
            {0,0}
        },
        {
            48, 0, 16, 16,
            {0,0},
            {0,0}
        },
    },
    {
        {
            16, 0, 8, 16,
            {0,0},
            {0,0}
        },
        {
            40, 0, 8, 16,
            {0,0},
            {0,0}
        }
    },
    {
        {
            232 + 0, 0, 16, 16,
            {0,0},
            {0,0}
        },
        {
            232 + 24, 0, 16, 16,
            {0,0},
            {0,0}
        },
        {
            232 + 48, 0, 16, 16,
            {0,0},
            {0,0}
        },
    },
    {
        {
            232 + 16, 0, 8, 16,
            {0,0},
            {0,0}
        },
        {
            232 + 40, 0, 8, 16,
            {0,0},
            {0,0}
        }
    }
};

static movie_prog_uipara_t uipara_720_576 =
{
    //pipe
    1,
    //uipara_lyr
    {
        (720 - 457) / 2 - 100, 40, 457 + 200, 20,
        {0,0},
        {0,0}
    },
    //uipara_frm
    {
        0, 0, 457 + 200, 20,
        {0,0},
        {0,0}
    },
    //uipara_bg
    {
        0, 0, 480, 70,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0}
    },
    //uipara_icon
    {
        {
            100, 0, 457, 20,
            {ID_MOVIE_SPSC_PROG_BG_BMP,0},
            {0,0}
            //79, 65-46-10, 680, 26,
            //ID_INIT_PROG_BG_BMP
        },
        {
            100, 0, 12, 20,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0}
            //79, 65-46-10, 12, 26,
            //ID_INIT_PROG_LEFT_BMP
        },
        {
            100, 0, 12, 20,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0}
            // 79, 65-46-10, 12, 26,
            //ID_INIT_PROG_MID_BMP
        },
        {
            100, 0, 24, 20,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0}
            //79, 65-46-10, 26, 26,
            //ID_INIT_PROG_CURSOR_BMP
        },
    },

    //uipara_prog_cur_time
    {
        {
            0, 0, 25, 20,
            {0,0},
            {0,0}
        },
        {
            38, 0, 25, 20,
            {0,0},
            {0,0}
        },
        {
            75, 0, 25, 20,
            {0,0},
            {0,0}
        },
    },
    //uipara_prog_cur_time_dot
    {
        {
            25, 0, 12, 20,
            {0,0},
            {0,0}
        },
        {
            63, 0, 12, 20,
            {0,0},
            {0,0}
        }
    },
    //uipara_prog_total_time
    {
        {
            557 + 3 + 0, 0, 25, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 3 + 38, 0, 25, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 3 + 75, 0, 25, 20,
            {0,0},
            {0,0}
        },
    },
    //uipara_prog_total_time_dot
    {
        {
            557 + 3 + 25, 0, 12, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 3 + 63, 0, 12, 20,
            {0,0},
            {0,0}
        }
    }
};

static movie_prog_uipara_t uipara_720_480 =
{
    //pipe
    1,
    //uipara_lyr
    {
        (720 - 457) / 2 - 110, 40, 457 + 220, 20,
        {0,0},
        {0,0}
    },
    //uipara_frm
    {
        0, 0, 457 + 220, 20,
        {0,0},
        {0,0}
    },
    //uipara_bg
    {
        0, 0, 480, 70,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0}
    },
    //uipara_icon
    {
        {
            110, 0, 457, 20,
            {ID_MOVIE_SPSC_PROG_BG_BMP,0},
            {0,0}
            //79, 65-46-10, 680, 26,
            //ID_INIT_PROG_BG_BMP
        },
        {
            110, 0, 12, 20,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0}
            //79, 65-46-10, 12, 26,
            //ID_INIT_PROG_LEFT_BMP
        },
        {
            110, 0, 12, 20,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0}
            // 79, 65-46-10, 12, 26,
            //ID_INIT_PROG_MID_BMP
        },
        {
            110, 0, 24, 20,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0}
            //79, 65-46-10, 26, 26,
            //ID_INIT_PROG_CURSOR_BMP
        },
    },

    //uipara_prog_cur_time
    {
        {
            0, 0, 27, 20,
            {0,0},
            {0,0}
        },
        {
            40, 0, 27, 20,
            {0,0},
            {0,0}
        },
        {
            77, 0, 27, 20,
            {0,0},
            {0,0}
        },
    },
    //uipara_prog_cur_time_dot
    {
        {
            27, 0, 12, 20,
            {0,0},
            {0,0}
        },
        {
            67, 0, 12, 20,
            {0,0},
            {0,0}
        }
    },
    //uipara_prog_total_time
    {
        {
            557 + 12 + 0, 0, 27, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 12 + 40, 0, 27, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 12 + 77, 0, 27, 20,
            {0,0},
            {0,0}
        },
    },
    //uipara_prog_total_time_dot
    {
        {
            557 + 12 + 27, 0, 12, 20,
            {0,0},
            {0,0}
        },
        {
            557 + 12 + 67, 0, 12, 20,
            {0,0},
            {0,0}
        }
    }

};

static movie_prog_uipara_t uipara_800_480 =
{
    //pipe
    1,
    //uipara_lyr
    {
        0, 40, 800, 56,
        {0,0},
        {0,0},
    },
    //uipara_frm
    {
        0, 0, 800, 56,
        {0,0}, 
        {0,0},
    },
    //uipara_bg
    {
        0, 0, 800, 56,
        {ID_MOVIE_SPSC_BG_BMP,0},
        {0,0}
    },

    //uipara_icon
    {
        {
            (800 - 457) / 2, 18, 457, 20,
            {ID_MOVIE_SPSC_PROG_BG_BMP,0},
            {0,0}
        },
        {
            (800 - 457) / 2, 18, 12, 20,
            {ID_MOVIE_PROG_LEFT_BMP,0},
            {0,0}
        },
        {
            (800 - 457) / 2, 18, 12, 20,
            {ID_MOVIE_PROG_MID_BMP,0},
            {0,0}
        },
        {
            (800 - 457) / 2, 18, 24, 20,
            {ID_MOVIE_PROG_CURSOR_BMP,0},
            {0,0}
        },
    },

    //uipara_prog_cur_time
    {
        {
            (800 - 457) / 2 - (30 * 3 + 2 * 2), 18, 30, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 - (30 * 2 + 2), 18, 30, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 - 30, 18, 30, 20,
            {0,0},
            {0,0},
        },
    },
    //uipara_prog_cur_time_dot
    {
        {
            (800 - 457) / 2 - (30 * 2 + 2 * 2), 18, 2, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 - (30 + 2), 18, 2, 20,
            {0,0},
            {0,0},
        }
    },
    //uipara_prog_total_time
    {
        {
            (800 - 457) / 2 + 457, 18, 30, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 + 457 + (30 + 2), 18, 30, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 + 457 + (30 * 2 + 2 * 2), 18, 30, 20,
            {0,0},
            {0,0},            
        },
    },
    //uipara_prog_total_time_dot
    {
        {
            (800 - 457) / 2 + 457 + 30, 18, 2, 20,
            {0,0},
            {0,0},
        },
        {
            (800 - 457) / 2 + 457 + (30 * 2 + 2), 18, 2, 20,
            {0,0},
            {0,0},            
        }
    }
};


movie_prog_uipara_t *movie_prog_get_uipara(__s32 rotate)
{
    __s32           screen_width;
    __s32           screen_height;
    /* get lcd size*/
    __msg("++++++%s, %d+++++++\n", __FILE__, __LINE__);
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
            else if ((screen_width == 720) && (screen_height == 576))
            {
                return &uipara_720_576;
            }
            else if ((screen_width == 720) && (screen_height == 480))
            {
                return &uipara_720_480;
            }
            else if ((screen_width == 800) && (screen_height == 480))
            {
                eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
                return &uipara_800_480;
            }
        }

        case GUI_SCNDIR_ROTATE90:
        case GUI_SCNDIR_ROTATE270:
            break;
    }

    return &uipara_800_480;
}

