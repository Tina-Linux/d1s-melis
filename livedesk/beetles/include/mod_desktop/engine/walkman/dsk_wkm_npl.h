/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_npl.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.06
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.06        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_NPL_H__
#define  __DSK_WKM_NPL_H__

#ifdef __cplusplus
extern "C" {
#endif



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_add_item
*Prototype   : __s32  dsk_wkm_npl_add_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : add a file to now playlist
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_add_item(const char *file);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_delete_item
*Prototype   : __s32  dsk_wkm_npl_delete_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : delete a file from now playlist
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_delete_item(const char *file);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_clear
*Prototype   : __s32  dsk_wkm_npl_clear( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : clear the now playlist
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_clear(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_set_cur
*Prototype   : __s32  dsk_wkm_npl_set_cur( __u32 index )
*Arguments   : index     input. the index of the item that the playing ponter points to.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the index of the item that the playing ponter points to. Note : The function is
*              only valid when the walkman is NOT in playing status.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_set_cur(__u32 index);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_cur
*Prototype   : __s32  dsk_wkm_npl_get_cur( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. -1 means fail.
*Description : get the index of the item that the playing ponter points to.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_get_cur(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_next
*Prototype   : __s32  dsk_wkm_npl_get_next( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. -1 means fail.
*Description : get the index of the item that the playing ponter points to.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_get_next(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_file2index
*Prototype   : __s32  dsk_wkm_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. -1 means fail.
*Description : get the index value of the file in the now play list.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_file2index(const char *file);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_index2file
*Prototype   : __s32  dsk_wkm_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_index2file(__u32 index, char *file);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_total_count
*Prototype   : __s32  dsk_wkm_npl_get_total_count( void )
*Arguments   : void
*Return      : the total count of items in the now play list. -1 means fail.
*Description : get the total count of items in the now play list.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_get_total_count(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_save_list
*Prototype   : __s32  dsk_wkm_npl_save_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : save the now play list to data disk
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_save_list(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_load_last_list
*Prototype   : __s32  dsk_wkm_npl_load_last_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : load the last play list to the npl.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_npl_load_last_list(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_NPL_H__

/* end of dsk_wkm_npl.h */
