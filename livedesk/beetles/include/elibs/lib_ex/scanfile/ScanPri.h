/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook information manager libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : ScanPri.h
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-22 10:07:36
*********************************************************************************************************
*/

#ifndef     __SCAN_PRI_H__
#define     __SCAN_PRI_H__

#include "OAL.h"
#include "Scan.h"
#include "ScanStack.h"

/* the max length of ebook file path */
#define SCAN_MAX_PATH       (768+1)
#define SCAN_MAX_DEPTH      20
#define FILELIST_LEN        4096

/*
 * store scaned file entry information,
 * the name of file not use fixed length to store,
 * this can reduce usage of memory.
 */
typedef struct ScanFileEntry
{
    __u32               RecLen;     /* length of scan file entry    */
    __u32               Prev;       /* prev file entry offset within file list data areas */
    __u64               Format;     /* format of file           */
    __u8                Name[1];    /* file path, the length depend on RecLen, NOTE : Name must been located at the last*/
} ScanFileEntry_t;

/*
 * store scaned file information list,
 * the name of file not use fixed length to store,
 * this can reduce usage of memory.
 */
typedef struct ScanFileList
{

    struct ScanFileList *Prev;
    struct ScanFileList *Next;

    __u32               Alloc;      /* allocation size of data area             */
    __u32               Size;       /* size of useful data area                 */
    __u32               Top;        /* begin location of the last insert entry  */
    __u8                Data[1];    /* data area of file entries                */
} ScanFileList_t;

typedef struct ScanListCursor
{
    ScanFileList_t      *CurList;
    __u32               Offset;     /* current file entry offet within data area*/
} ScanListCursor_t;

/*
 * to store valid ebook format mapping informations
 */
typedef struct FmtIdx
{
    const char          *Suffix;
    int                 Format;
    __u64               ScanFlag;
} FmtIdx_t;

/*
 * to store scan result and control scan process.
 */
typedef struct ScanInfo
{

    ScanFileList_t      *FirstList; /* point to the first scaned file list  */
    ScanFileList_t      *LastList;  /* point to the last scaned file list   */
    ScanListCursor_t    ScanCursor; /* point to the current scaned file list*/

    ScanListCursor_t    TmpCursor;  /* point to the tmp cursor*/

    __u64               ScanTypes;  /* user want to scan file types         */
    int                 OnceCnt;    /* read items number when read once     */
    int                 CurCnt;     /* current scaned file count            */

    HSTACK              hStack;     /* stack to save sub-directory path     */
    ES_DIR              *pCurDir;   /* handle to current read directory     */
    char                CurPath[SCAN_MAX_PATH]; /* current open directory   */
    char                tempPath[SCAN_MAX_PATH];/* for scan temp use        */

    ScanedEntry_t       OutEntry;   /* return out entry                     */
    ScanedEntry_t       OutEntryCursor;     /* return out entry             */
} ScanInfo_t;

/*************************************************/
//private:
int __ScanMoveCursorToNext(HSCAN hScan, ScanListCursor_t *cursor);
int __ScanMoveCursorToFirst(HSCAN hScan, ScanListCursor_t *cursor);
int __ScanMoveCurosrToPrev(HSCAN hScan, ScanListCursor_t *cursor);
ScanedEntry_t *__ScanGetCurrentCursorData(HSCAN hScan, ScanListCursor_t *cursor);
int       ScanFilesOneTime(ScanInfo_t *pScanInfo);
#endif      /* __SCAN_PRI_H__ */
