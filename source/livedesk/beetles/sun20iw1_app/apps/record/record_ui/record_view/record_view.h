#ifndef __RECORD_VIEW_H__
#define __RECORD_VIEW_H__

#include "record_ui.h"

#include "record_view_doc.h"

typedef enum RECORD_UPDATE_AREA
{
    RECORD_UPDATE_AREA_MENU,
    RECORD_UPDATE_AREA_ANI,
    RECORD_UPDATE_AREA_QUALIT,
} UPDATE_AREA;

typedef struct tag_record_view
{
    precord_ui_t    pui;

    RECORD_MENU     curMenu;
    FOCUSE_STATE    curMenuSt;
    __s16           menuOsffset;

    precord_doc_t   precord_doc;

} record_view_t, *precord_view_t;


extern void record_view_show(precord_view_t prec_view, H_WIN layer);
extern void record_updateview_show(precord_view_t prec_view, H_WIN layer);
extern void record_timer_view_show(precord_view_t prec_view, H_WIN layer);
//extern void record_view_menuBox( precord_view_t prec_view );





















#endif//__RECORD_VIEW_H__
