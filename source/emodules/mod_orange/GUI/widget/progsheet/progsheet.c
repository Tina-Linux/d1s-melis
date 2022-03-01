/*
*******************************************************************************************************************
*                                                           ProgSheet  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    ProgSheet.c
*
*Author：       Terry Zeng
*
*Version :      1.1.0
*
*Date:          2009-09-02
*
*Description :  ProgSheet control implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* Terry.Zeng     2009-09-02         1.1.0          Create File
*
*******************************************************************************************************************
*/

#include "progsheet_i.h"
#include <kconfig.h>

static __u32  last_key = 0xffffffff;
static __s32 ProgSheetCtrlProc(__gui_msg_t *msg);
/*
**********************************************************************************************************************
*                                               RegisterProgSheetControl
*
* author:
*
* date:             2009-09-02
*
* Description:      Register ProgSheet  Control to wight system
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__bool RegisterProgSheetControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_PROGSHEET;
    WndClass.dwStyle     = WS_NONE | WS_MEMDEV;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = ProgSheetCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               create_page
*
* author:
*
* date:             2009-09-02
*
* Description:      create page elements
*
* parameters:       pData： progsheet data； page：page data；msg：gui information
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 create_page(progsheet_data_t *pData, page_t *page, __gui_msg_t *msg)
{
    page_para_t *page_data;
    __gui_ctlwincreate_para_t   ctlcreate_info;
    __gui_msg_t my_msg;
    //__u32 style;
    /*从msg->dwAddData1拿到page的控件*/
    page_data = (page_para_t *)msg->dwAddData1;
    /*page的回调函数*/
    page->proc = (__pGUI_WIN_CB)msg->dwAddData2;
    page->data = (__u32)page_data;
    eLIBs_memset(&ctlcreate_info, 0, sizeof(__gui_ctlwincreate_para_t));
    /*创建page对应的static窗口，窗口属性为不可见*/
    ctlcreate_info.name = page_data->text;
    ctlcreate_info.spClassName = CTRL_STATIC;
    ctlcreate_info.dwStyle = WS_NONE;
    ctlcreate_info.id = pData->page_count + 1;
    GUI_WinGetClientRECT(msg->h_deswin, &ctlcreate_info.ctl_rect);
    ctlcreate_info.hParent = msg->h_deswin;
    ctlcreate_info.attr = (void *)page_data;
    page->hwnd = GUI_CtrlWinCreate(&ctlcreate_info);
    /*设置page对应static对应的回调函数*/
    GUI_WinSetCallback(page->hwnd, page->proc);
    /*向page对应的static窗口发送GUI_MSG_CREATE同步消息*/
    //  style = GUI_WinGetStyle(page->hwnd);
    //    style = style |WS_VISIBLE;
    //    GUI_WinSetStyle(page->hwnd,style);
    my_msg.id = GUI_MSG_CREATE;
    my_msg.h_srcwin = msg->h_deswin;
    my_msg.h_deswin = page->hwnd;
    my_msg.p_arg    = NULL;
    GUI_SendMessage(&my_msg);
    return 0;
}

/*
**********************************************************************************************************************
*                                               append_page
*
* author:
*
* date:             2009-09-02
*
* Description:      append page to progsheet
*
* parameters:       pData： progsheet data； page：page data；
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 append_page(progsheet_data_t *pData, page_t *new_page)
{
    int i = 0;
    page_t *page;
    page = pData->head;

    while (page && page->next)
    {
        i++;
        page = page->next;
    }

    new_page->next = NULL;

    if (page)
    {
        i++;
        page->next = new_page;
    }
    else
    {
        pData->head = new_page;
        pData->head->next = NULL;
    }

    pData->active_page = new_page;
    pData->page_count++;
    return i;
}

/*
**********************************************************************************************************************
*                                               calc_page_title_width
*
* author:
*
* date:             2009-09-02
*
* Description:      calc_page_title_width
*
* parameters:       page：page data；
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 calc_page_title_rect(progsheet_data_t *pData, page_t *page, __gui_msg_t *msg)
{
    //  __s32 width;
    //  page_t *end_page;
    //  page_para_t *data = (page_para_t *)page->data;
    //  width = data->head.width;
    //  page->rect.width = width;
    //  page->rect.height = data.height;
    //
    //  end_page = pData->head;
    //  while(end_page != NULL && end_page->next != NULL)
    //  {
    //      __inf("\n%s,%d",__FILE__,__LINE__);
    //      end_page = end_page->next;
    //
    //  }
    //  if(end_page == NULL)
    //  {
    //      end_page->rect.x = ((progsheet_para_t *)pData->data)->head_rect.x;
    //  }
    //  page->rect.x = end_page->rect.x + end_page->rect.width;
    //  page->rect.y = ((progsheet_para_t *)pData->data)->head_rect.y;
    return 0;
}
/*
**********************************************************************************************************************
*                                               ProgSheetCtrlProc
*
* author:
*
* date:             2009-09-02
*
* Description:      ProgSheet Control Process
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
static __s32 ProgSheetCtrlProc(__gui_msg_t *msg)
{
    __s32 result = 0;
    progsheet_data_t *pData;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            pData = (progsheet_data_t *)orange_malloc(sizeof(progsheet_data_t));

            if (pData == NULL)
            {
                result = ORANGE_FAIL;
                break;
            }

            eLIBs_memset(pData, 0, sizeof(progsheet_data_t));
            pData->page_count = 0;
            pData->active_page = NULL;
            pData->head = NULL;
            pData->hmem = 0;
            /*progsheet对应的头部区域，也即page标题栏所在的区域*/
            pData->data = (__u32)GUI_WinGetAttr(msg->h_deswin);
            /*把progsheet当成窗口的私有属性传递给窗口*/
            GUI_CtrlWinSetAddData(msg->h_deswin, (__u32)pData);
            break;
        }

        case GUI_MSG_DESTROY:
        {
            pData = (progsheet_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->hmem != 0)
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                GUI_MEMDEV_Delete(pData->hmem);
#endif
                pData->hmem = 0;
            }

            orange_mfree(pData);
            break;
        }

        case PROGSHEET_ADD_PAGE:
        {
            page_t *page;
            __gui_msg_t my_msg;
            page = (page_t *)orange_malloc(sizeof(page_t));

            if (page == NULL)
            {
                return -1;
            }

            eLIBs_memset(page, 0, sizeof(page_t));
            /*从窗口的私有属性里获取回progsheet数据*/
            pData = (progsheet_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            /*通知先前的page显示隐藏*/
            if (pData->active_page != NULL)
            {
                my_msg.id = GUI_MSG_DESTROY;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.h_deswin = pData->active_page->hwnd;
                my_msg.dwAddData1 = 0;
                GUI_SendMessage(&my_msg);
            }

            /*创建page的元素*/
            result = create_page(pData, page, msg);

            if (result == -1)
            {
                break;
            }

            /*把page添加到sheet里*/
            append_page(pData, page);
            /*计算page title的宽度*/
            //          calc_page_title_rect(pData,page,msg);
            /*重新绘制progsheet*/
            GUI_InvalidateRect(msg->h_deswin, NULL, ORANGE_TRUE);
            GUI_CtrlWinSetAddData(msg->h_deswin, (__u32)pData);
            break;
        }

        case PROGSHEET_SET_ACTIVEINDEX:
        {
            __s32 index;
            page_t *page;
            page_t *temp;
            __gui_msg_t my_msg;
            pData = (progsheet_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            page = pData->head;
            temp = pData->head;
            /*判断当前的活动page是否改变*/
            index = msg->dwAddData1;

            while ((temp != NULL) && (index >= 0))
            {
                page = temp;
                temp = temp->next;
                index--;
            }

            /*改变时*/
            if (page != pData->active_page)
            {
                /*通知先前的page显示隐藏*/
                my_msg.id = GUI_MSG_DESTROY;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.h_deswin = pData->active_page->hwnd;
                my_msg.dwAddData1 = 0;
                GUI_SendMessage(&my_msg);
                /*则设置当前活动page为新的page*/
                pData->active_page = page;
                /*重新绘制progsheet*/
                GUI_InvalidateRect(msg->h_deswin, NULL, ORANGE_TRUE);
            }

            break;
        }

        case GUI_MSG_PAINT:
        {
            __gui_rect_t    rcrect;
            RECT            fbrect;
            RECT            headrect;
            page_t *page;
            page_para_t *page_data;
            __gui_msg_t my_msg;
            pData = (progsheet_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            /*绘制progsheet及page*/
            GUI_LyrWinSel(GUI_LyrP2H(GUI_WinGetLyrWin(msg->h_deswin)));
            GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);

            if (!pData->hmem)
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                pData->hmem = GUI_MEMDEV_Create(fbrect.x, fbrect.y, fbrect.width, fbrect.height);
#endif

                __inf("\n%s,%d GUI_MEMDEV_Create ERROR!", __FILE__, __LINE__);
                return -1;
            }

            SysRectToGuiRect(&fbrect, &rcrect);
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
            GUI_MEMDEV_Select(pData->hmem);           // start memory device
#endif
            /*绘制属性表*/
            GUI_SetBkColor(0);
            GUI_ClearRect(rcrect.left, rcrect.top, rcrect.right, rcrect.bottom);
            GUI_BMP_Draw(((progsheet_para_t *)pData->data)->bmp, rcrect.left + ((progsheet_para_t *)pData->data)->bmp_pos.x, rcrect.top + ((progsheet_para_t *)pData->data)->bmp_pos.y);
            GUI_CharSetToEncode(((progsheet_para_t *)pData->data)->draw_code);
            GUI_SetFont(((progsheet_para_t *)pData->data)->draw_font);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_DispStringAt(((progsheet_para_t *)pData->data)->text, rcrect.left + ((progsheet_para_t *)pData->data)->text_pos.x, rcrect.top + ((progsheet_para_t *)pData->data)->text_pos.y);
            /*分别绘制属性页*/
            page = pData->head;

            while (page)
            {
                headrect.x = fbrect.x + (((page_para_t *)page->data)->head).x;
                headrect.y = fbrect.y + (((page_para_t *)page->data)->head).y;
                headrect.width = (((page_para_t *)page->data)->head).width;
                headrect.height = (((page_para_t *)page->data)->head).height;
                SysRectToGuiRect(&headrect, &rcrect);
                GUI_SetBkColor(0);
                GUI_ClearRect(rcrect.left, rcrect.top, rcrect.right, rcrect.bottom);
                page_data = (page_para_t *)page->data;

                if (page != pData->active_page)
                {
                    GUI_BMP_Draw(page_data->unfocus_bmp, rcrect.left + page_data->bmp_pos.x, rcrect.top + page_data->bmp_pos.y);
                }
                else
                {
                    GUI_BMP_Draw(page_data->focus_bmp, rcrect.left + page_data->bmp_pos.x, rcrect.top + page_data->bmp_pos.y);
                }

                GUI_CharSetToEncode(page_data->draw_code);
                GUI_SetFont(page_data->draw_font);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

                if (page != pData->active_page)
                {
                    GUI_SetColor(page_data->uftxt_color);
                    GUI_DispStringAt(page_data->text, rcrect.left + page_data->text_pos.x, rcrect.top + page_data->text_pos.y);
                }
                else
                {
                    GUI_SetColor(page_data->ftxt_color);
                    GUI_DispStringAt(page_data->text, rcrect.left + page_data->text_pos.x, rcrect.top + page_data->text_pos.y);
                }

                page = page->next;
            }

#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
            GUI_MEMDEV_CopyToLCD(pData->hmem);            // copy from memory device to fb
            GUI_MEMDEV_Select(0);
#endif

            if (pData->active_page)
            {
                my_msg.id = PROGSHEET_PAINT_PAGE;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.h_deswin = pData->active_page->hwnd;
                my_msg.p_arg    = NULL;
                GUI_SendMessage(&my_msg);
            }                 // stop memory device

            break;
        }

        case BM_CLICK:
        {
            __gui_notify_msg_t      notify_msg;
            notify_msg.hWnd     = msg->h_deswin;
            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
            notify_msg.msgcode  = BN_CLICKED;
            notify_msg.dwAddData = 0;
            GUI_NotifyParent(&notify_msg);
        }

        case GUI_MSG_KEY:
        {
            switch (msg->dwAddData1)
            {
                /*case GUI_MSG_KEY_RISE:
                {
                    __gui_notify_msg_t      notify_msg;

                    notify_msg.hWnd     = msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode  = BN_CLICKED;
                    notify_msg.dwAddData= last_key;

                    GUI_NotifyParent (&notify_msg);
                    break;
                }*/
                case GUI_MSG_KEY_DOWN:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_CLICKED;
                        notify_msg.dwAddData = last_key;
                        GUI_NotifyParent(&notify_msg);
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_PUSHED;
                        notify_msg.dwAddData = GUI_MSG_KEY_DOWN;
                        GUI_NotifyParent(&notify_msg);
                    }

                    break;
                }

                case GUI_MSG_KEY_ENTER:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_CLICKED;
                        notify_msg.dwAddData = last_key;
                        GUI_NotifyParent(&notify_msg);
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_PUSHED;
                        notify_msg.dwAddData = GUI_MSG_KEY_ENTER;
                        GUI_NotifyParent(&notify_msg);
                    }

                    break;
                }

                case GUI_MSG_KEY_RIGHT:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_UP:
                default:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_CLICKED;
                        notify_msg.dwAddData = last_key;
                        GUI_NotifyParent(&notify_msg);
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = BN_PUSHED;
                        notify_msg.dwAddData = msg->dwAddData1;
                        GUI_NotifyParent(&notify_msg);
                    }

                    break;
                }
            }

            /*if(msg->dwAddData1 != GUI_MSG_KEY_RISE)
            {
                last_key = msg->dwAddData1;
            }
            else
            {
                last_key = 0xffffffff;
            }*/
            last_key = msg->dwAddData1;
            break;
        }

        case GUI_MSG_TOUCH:
        {
            __s32 x, y;
            __gui_msg_t my_msg;
            page_t *page;
            //  __gui_notify_msg_t      notify_msg;
            __u32 style;
            pData = (progsheet_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            x = LOSWORD(msg->dwAddData2);
            y = HISWORD(msg->dwAddData2);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_UP:
                {
                    //                  GUI_WinSetCaptureWin(msg->h_deswin);
                    page = pData->head;

                    while (page)
                    {
                        if (((((page_para_t *)page->data)->head).x < x) && (x < ((((page_para_t *)page->data)->head).x + (((page_para_t *)page->data)->head).width)))
                        {
                            if (((((page_para_t *)page->data)->head).y < y) && (y < ((((page_para_t *)page->data)->head).y + (((page_para_t *)page->data)->head).height)))
                            {
                                break;
                            }
                        }

                        page = page->next;
                    }

                    if (page == pData->active_page)
                    {
                        break;
                    }

                    if (page != NULL)
                    {
                        /*通知先前的page显示隐藏*/
                        my_msg.id = GUI_MSG_DESTROY;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.h_deswin = pData->active_page->hwnd;
                        my_msg.dwAddData1 = 0;
                        GUI_SendMessage(&my_msg);
                        /*则设置当前活动page为新的page*/
                        pData->active_page = page;
                        style = GUI_WinGetStyle(pData->active_page->hwnd);
                        style = style & (~WS_VISIBLE);
                        GUI_WinSetStyle(pData->active_page->hwnd, style);
                        my_msg.id = GUI_MSG_CREATE;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.h_deswin = pData->active_page->hwnd;
                        my_msg.dwAddData1 = 0;
                        GUI_SendMessage(&my_msg);
                        /*重新绘制progsheet*/
                        GUI_InvalidateRect(msg->h_deswin, NULL, ORANGE_TRUE);
                        //                      notify_msg.hWnd     = msg->h_deswin;
                        //                      notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        //                      notify_msg.msgcode  = PROGSHEET_PAGE_CHANGE;
                        //                      notify_msg.dwAddData= msg->dwAddData1;
                        //
                        //                      GUI_NotifyParent (&notify_msg);
                    }
                    else
                    {
                        my_msg.id = msg->id;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.h_deswin = pData->active_page->hwnd;
                        my_msg.dwAddData1 = msg->dwAddData1;
                        my_msg.dwAddData2 = msg->dwAddData2;
                        GUI_SendMessage(&my_msg);
                    }

                    break;
                }

                case GUI_MSG_TOUCH_MOVE:
                {
                    break;
                }

                case GUI_MSG_TOUCH_DOWN:
                {
                    page = pData->head;

                    while (page)
                    {
                        if (((((page_para_t *)page->data)->head).x < x) && (x < ((((page_para_t *)page->data)->head).x + (((page_para_t *)page->data)->head).width)))
                        {
                            if (((((page_para_t *)page->data)->head).y < y) && (y < ((((page_para_t *)page->data)->head).y + (((page_para_t *)page->data)->head).height)))
                            {
                                break;
                            }
                        }

                        page = page->next;
                    }

                    if (page != NULL)
                    {
                        break;
                    }
                    else
                    {
                        my_msg.id = msg->id;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.h_deswin = pData->active_page->hwnd;
                        my_msg.dwAddData1 = msg->dwAddData1;
                        my_msg.dwAddData2 = msg->dwAddData2;
                        GUI_SendMessage(&my_msg);
                    }

                    //                  if (GUI_WinGetCaptureWin () != msg->h_deswin)
                    //                  {
                    //                      break;
                    //                  }
                    //                  GUI_WinSetCaptureWin(msg->h_deswin);
                    //                  page = pData->active_page;
                    //
                    //                  notify_msg.hWnd     = msg->h_deswin;
                    //                  notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    //                  notify_msg.msgcode  = BN_CLICKED;
                    //                  notify_msg.dwAddData= msg->dwAddData1;
                    //
                    //                  GUI_NotifyParent (&notify_msg);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        default:
        {
            result = GUI_CtrlWinDefaultProc(msg);
            break;
        }
    }

    return result;
}
