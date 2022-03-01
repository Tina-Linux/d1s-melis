#ifndef __RESADAPTER_H__E31A9326_261C_411D_BCD5_BCD98EB8391E__
#define __RESADAPTER_H__E31A9326_261C_411D_BCD5_BCD98EB8391E__

//////////////////////////////////////////////////////////////////////////
//读资源方式改变，为减小(移植)代码修改，使用以下宏
typedef unsigned long HTHEME;

#define DECLARE_BMP_RES_Pt(_name)   \
    void* _name; \
    HTHEME _name ## _h

#define Create_BMP_RES_Pt(_p, _id)   \
    do {_p ## _h = dsk_theme_open(_id); _p = dsk_theme_hdl2buf(_p ## _h);}while(0)

#define Destroy_BMP_RES_Pt(_p)       dsk_theme_close(_p ## _h)
//////////////////////////////////////////////////////////////////////////

#endif //__RESADAPTER_H__E31A9326_261C_411D_BCD5_BCD98EB8391E__
//End of this file

