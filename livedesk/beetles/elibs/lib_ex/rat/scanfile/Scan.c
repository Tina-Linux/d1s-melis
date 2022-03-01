/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook information manager libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : Scan.c
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-22 13:54:03
*********************************************************************************************************
*/
#include <typedef.h>
#include <log.h>
#include <libc.h>
#include "OAL.h"
#include "ScanPri.h"
#include "rat_common.h"
#include <kconfig.h>
#include <kapi.h>

#define LIST_DATA_OFFSET(list)      ((int)((list)->Data - (__u8 *)(list)))
#define ENTRY_NAME_OFFSET(entry)    ((int)((entry)->Name - (__u8 *)(entry)))

static int  __ScanAllFiles(ScanInfo_t *pScanInfo);
static int  __SetupScanProcess(ScanInfo_t *pScanInfo);
static void __DestoryScanInfo(ScanInfo_t *pScanInfo);
__bool      __ValidFileType(const char *file, __u64 Types, int *Format);
static int  __InsertFileEntryToList(const char *filePath, int format, ScanInfo_t *pScanInfo);

prat_search_cb  g_rat_search_cb     = NULL;
void            *g_rat_search_arg   = NULL;


/*
****************************************************************************************************
*
*                                       ScanFilesBegin
*
*  Description:
*               -   begin the scan process.
*
*  Parameters:
*   pDir        -   pointer to scan directory.
*   FileTypes   -   types of scan ebook.
*   OnceCnt     -   read items number when read once.
*                   if zero, I will read all items one time.
*
*  Return value:
*   NULL        -   setup scan process failed.
*   others      -   handle to scan process.
****************************************************************************************************
*/
HSCAN ScanFilesBegin(const char *pDir, __u64 FileTypes, int OnceCnt)
{
    ScanInfo_t      *pScanInfo;
    ScanFileList_t  *pList;
    HSTACK          hStack;

    if (NULL == pDir)
    {
        __wrn("invalid paramter for ScanBookFile");
        return NULL;
    }

    pScanInfo   = (ScanInfo_t *)rat_malloc(sizeof(ScanInfo_t));

    if (NULL == pScanInfo)
    {
        __wrn("malloc failed");
        return NULL;
    }

    pList = (ScanFileList_t *)rat_malloc(FILELIST_LEN);

    if (NULL == pList)
    {
        __wrn("malloc failed");
        rat_free(pScanInfo);
        return NULL;
    }

    /* initialize file list structure */
    pList->Prev         = NULL;
    pList->Next         = NULL;
    pList->Alloc        = FILELIST_LEN - LIST_DATA_OFFSET(pList);
    pList->Size         = 0;
    pList->Top          = 0;
    /* initialize scan cursor structure */

    hStack = ScanStackAlloc();

    if (NULL == hStack)
    {
        __wrn("stack allocate failed");
        rat_free(pList);
        rat_free(pScanInfo);
        return NULL;
    }

    pScanInfo->FirstList    = pList;
    pScanInfo->LastList     = pList;
    pScanInfo->ScanCursor.CurList   = pList;
    pScanInfo->ScanCursor.Offset    = 0;
    pScanInfo->ScanTypes    = FileTypes;
    pScanInfo->OnceCnt      = OnceCnt;
    pScanInfo->CurCnt       = 0;
    pScanInfo->hStack       = hStack;
    pScanInfo->pCurDir      = NULL;

    eLIBs_strcpy(pScanInfo->CurPath, pDir);

    pScanInfo->OutEntry.Format  = RAT_MEDIA_TYPE_UNKNOWN;
    pScanInfo->OutEntry.Path    = NULL;

    /* setup scan work process */
    if (EPDK_OK != __SetupScanProcess(pScanInfo))
    {
        __DestoryScanInfo(pScanInfo);
        __wrn("setup scan process failed");
        return NULL;
    }

    return (HSCAN)pScanInfo;
}


/*
****************************************************************************************************
*
*                                       ScanMoveToFirst
*
*  Description:
*               -   move scan handle cursor position to first entry.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   EPDK_OK     -   move succeeded.
*   EPDK_FAIL   -   move failed.
****************************************************************************************************
*/
int ScanMoveToFirst(HSCAN hScan)
{
    ScanInfo_t *pScanInfo;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)hScan;
    pScanInfo->ScanCursor.CurList = pScanInfo->FirstList;
    pScanInfo->ScanCursor.Offset = 0;
    __msg("scan cursor move to first...");
    return EPDK_OK;
}

int __ScanMoveCursorToFirst(HSCAN hScan, ScanListCursor_t *cursor)
{
    ScanInfo_t *pScanInfo;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)hScan;
    cursor->CurList = pScanInfo->FirstList;
    cursor->Offset = 0;
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*                                       ScanMoveToNext
*
*  Description:
*               -   move scan handle cursor position to next entry.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   EPDK_OK     -   move succeeded.
*   EPDK_FAIL   -   move failed.
****************************************************************************************************
*/
int ScanMoveToNext(HSCAN hScan)
{
    ScanInfo_t      *pScanInfo;
    ScanFileEntry_t *pEntry;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)hScan;

    if ((pScanInfo->ScanCursor.Offset == pScanInfo->ScanCursor.CurList->Top))
    {
        if (NULL == pScanInfo->ScanCursor.CurList->Next)
        {
            int Count;
            /* the next entry have not read out, should read it by self */
            Count = ScanFilesOneTime(pScanInfo);

            if (0 == Count)
            {
                /* have no valid ebook files, scan is done */
                pScanInfo->ScanCursor.Offset = pScanInfo->ScanCursor.CurList->Size;
                return EPDK_FAIL;
            }

            if (pScanInfo->ScanCursor.Offset == pScanInfo->ScanCursor.CurList->Top)
            {
                /* the read out entry not located in current file list */
                pScanInfo->ScanCursor.CurList = pScanInfo->ScanCursor.CurList->Next;
                pScanInfo->ScanCursor.Offset = 0;
                return EPDK_OK;
            }
        }
        else
        {
            pScanInfo->ScanCursor.CurList = pScanInfo->ScanCursor.CurList->Next;
            pScanInfo->ScanCursor.Offset = 0;
            return EPDK_OK;
        }
    }

    pEntry = (ScanFileEntry_t *)(pScanInfo->ScanCursor.CurList->Data + \
                                 pScanInfo->ScanCursor.Offset);
    pScanInfo->ScanCursor.Offset += pEntry->RecLen;
    return EPDK_OK;
}

int __ScanMoveCursorToNext(HSCAN hScan, ScanListCursor_t *cursor)
{
    ScanFileEntry_t *pEntry;

    if ((cursor->Offset == cursor->CurList->Top))
    {
        if (NULL == cursor->CurList->Next)
        {
            return EPDK_FAIL;
        }
        else
        {
            cursor->CurList = cursor->CurList->Next;
            cursor->Offset = 0;
            return EPDK_OK;
        }
    }

    pEntry = (ScanFileEntry_t *)(cursor->CurList->Data + cursor->Offset);
    cursor->Offset += pEntry->RecLen;
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*                                       ScanMoveToPrev
*
*  Description:
*               -   move scan handle cursor position to prev entry.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   EPDK_OK     -   move succeeded.
*   EPDK_FAIL   -   move failed.
****************************************************************************************************
*/
int ScanMoveToPrev(HSCAN hScan)
{
    ScanInfo_t      *pScanInfo;
    ScanFileEntry_t *pEntry;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)hScan;

    if (0 == (pScanInfo->ScanCursor.Offset))
    {
        if (NULL == (pScanInfo->ScanCursor.CurList->Prev))
        {
            /* have reached begin of file list */
            return EPDK_FAIL;
        }
        else
        {
            /* move to the last entry of current prev list */
            pScanInfo->ScanCursor.CurList = pScanInfo->ScanCursor.CurList->Prev;
            pScanInfo->ScanCursor.Offset = pScanInfo->ScanCursor.CurList->Top;
            return EPDK_OK;
        }
    }

    if ((pScanInfo->ScanCursor.CurList->Size) == (pScanInfo->ScanCursor.Offset))
    {
        pScanInfo->ScanCursor.Offset = pScanInfo->ScanCursor.CurList->Top;
        return EPDK_OK;
    }

    pEntry = (ScanFileEntry_t *)(pScanInfo->ScanCursor.CurList->Data + \
                                 pScanInfo->ScanCursor.Offset);
    pScanInfo->ScanCursor.Offset = pEntry->Prev;
    return EPDK_OK;
}

int __ScanMoveCurosrToPrev(HSCAN hScan, ScanListCursor_t *cursor)
{
    ScanFileEntry_t *pEntry;

    if (0 == (cursor->Offset))
    {
        if (NULL == (cursor->CurList->Prev))
        {
            /* have reached begin of file list */
            return EPDK_FAIL;
        }
        else
        {
            /* move to the last entry of current prev list */
            cursor->CurList = cursor->CurList->Prev;
            cursor->Offset = cursor->CurList->Top;
            return EPDK_OK;
        }
    }

    if ((cursor->CurList->Size) == (cursor->Offset))
    {
        cursor->Offset = cursor->CurList->Top;
        return EPDK_OK;
    }

    pEntry = (ScanFileEntry_t *)(cursor->CurList->Data + \
                                 cursor->Offset);
    cursor->Offset = pEntry->Prev;
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*                                       CursorIsDone
*
*  Description:
*               -   check scan handle cursor position pointing at the end entry.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   EPDK_TRUE   -   cursor position pointing the end entry.
*   EPDK_FALSE  -   cursor position not pointing at the end entry.
****************************************************************************************************
*/
__bool ScanIsDone(HSCAN hScan)
{
    ScanInfo_t *pScanInfo;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return EPDK_FALSE;
    }

    pScanInfo = (ScanInfo_t *)hScan;

    if ((EPDK_TRUE == ScanStackEmpty(pScanInfo->hStack)) && \
        (NULL == pScanInfo->pCurDir))
    {
        if ((pScanInfo->ScanCursor.CurList == pScanInfo->LastList) && \
            (pScanInfo->ScanCursor.Offset == pScanInfo->ScanCursor.CurList->Size))
        {
            return EPDK_TRUE;
        }
    }

    return EPDK_FALSE;
}

/*
****************************************************************************************************
*
*                                       ScanFree
*
*  Description:
*               -   free scan handle.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*               NONE
****************************************************************************************************
*/
void ScanFree(HSCAN hScan)
{
    __DestoryScanInfo((ScanInfo_t *)hScan);
}


/*
****************************************************************************************************
*
*                                       ScanGetCurrent
*
*  Description:
*               -   get current scaned file entry.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   NULL        -   get current entry failed.
*   others      -   pointer to scan cursor current entry.
****************************************************************************************************
*/
ScanedEntry_t *ScanGetCurrent(HSCAN hScan)
{
    ScanInfo_t      *pScanInfo;
    ScanFileEntry_t *pEntry;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return NULL;
    }

    pScanInfo = (ScanInfo_t *)hScan;

    if ((pScanInfo->ScanCursor.Offset == pScanInfo->ScanCursor.CurList->Size))
    {
        if (NULL == pScanInfo->ScanCursor.CurList->Next)
        {
            /* buffer have no data */
            return NULL;
        }
        else
        {
            pScanInfo->ScanCursor.CurList = pScanInfo->ScanCursor.CurList->Next;
            pScanInfo->ScanCursor.Offset = 0;
        }
    }

    pEntry = (ScanFileEntry_t *)(pScanInfo->ScanCursor.CurList->Data + \
                                 pScanInfo->ScanCursor.Offset);
    pScanInfo->OutEntry.Format = pEntry->Format;
    pScanInfo->OutEntry.Path   = (char *)(pEntry->Name);
    return &(pScanInfo->OutEntry);
}

ScanedEntry_t *__ScanGetCurrentCursorData(HSCAN hScan, ScanListCursor_t *cursor)
{
    ScanInfo_t      *pScanInfo;
    ScanFileEntry_t *pEntry;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return NULL;
    }

    pScanInfo = (ScanInfo_t *)hScan;

    if ((cursor->Offset == cursor->CurList->Size))
    {
        if (NULL == cursor->CurList->Next)
        {
            /* buffer have no data */
            return NULL;
        }
        else
        {
            cursor->CurList = cursor->CurList->Next;
            cursor->Offset = 0;
        }
    }

    pEntry = (ScanFileEntry_t *)(cursor->CurList->Data + \
                                 cursor->Offset);
    pScanInfo->OutEntryCursor.Format = pEntry->Format;
    pScanInfo->OutEntryCursor.Path   = (char *)(pEntry->Name);
    return &(pScanInfo->OutEntryCursor);
}

/*
****************************************************************************************************
*
*                                       ScanGetCurCnt
*
*  Description:
*               -   get current scaned file count.
*
*  Parameters:
*   hScan       -   handle to scan information.
*
*  Return value:
*   The number of current files number, -1 if failed.
****************************************************************************************************
*/
int ScanGetCurCnt(HSCAN hScan)
{
    ScanInfo_t *pScanInfo;

    if (NULL == hScan)
    {
        __wrn("invalid parameter");
        return -1;
    }

    pScanInfo = (ScanInfo_t *)hScan;
    return pScanInfo->CurCnt;
}


static int __SetupScanProcess(ScanInfo_t *pScanInfo)
{
    int     Count;
    char    *p;
    char    *pblash;
    char    str_root[RAT_MAX_FULL_PATH_LEN];

    /* push current path to stack, ready for scan files */
    p   = pScanInfo->CurPath;

    do
    {
        if (NULL == p)
        {
            break;
        }

        pblash = eLIBs_strchr(p, '/');

        if (NULL == pblash)
        {
            if (NULL != p && 0 != *p)
            {
                eLIBs_strcpy(str_root, p);
                ScanStackPush(pScanInfo->hStack, str_root);
                __wrn("search root=%s", str_root);
            }

            break;
        }
        else
        {
            eLIBs_strncpy(str_root, p, pblash - p);
            str_root[pblash - p] = 0;
            ScanStackPush(pScanInfo->hStack, str_root);
            __wrn("search root=%s", str_root);
            p = pblash + 1;
        }
    } while (1);

    if (0 == pScanInfo->OnceCnt)
    {
        /* scan all files at one time */
        __wrn("__ScanAllFiles..");

        Count   = __ScanAllFiles(pScanInfo);

        pScanInfo->CurCnt   = Count;
        __wrn("pScanInfo->CurCnt=%d..", pScanInfo->CurCnt);

        if (0 == Count)
        {
            __wrn("have not find any valid media file");
        }

        return EPDK_OK;
    }
    else
    {
        /* scan pScanInfo->OnceCnt files one time */
        __wrn("ScanFilesOneTime");
        Count   = ScanFilesOneTime(pScanInfo);

        if (0 == Count)
        {
            __wrn("have not find any valid ebook file");
        }

        return EPDK_OK;
    }
}


static int __ScanAllFiles(ScanInfo_t *pScanInfo)
{
    ES_DIRENT   *pDirent;
    int          Format = RAT_MEDIA_TYPE_UNKNOWN;
    int          Count  = 0;

    __wrn("__ScanAllFiles0");

    while (EPDK_TRUE != ScanStackEmpty(pScanInfo->hStack))
    {
        /* pop directory from stack and save to pScanInfo->CurPath */
        eLIBs_strcpy(pScanInfo->CurPath, ScanStackPop(pScanInfo->hStack));

        pScanInfo->pCurDir  = esFSYS_opendir(pScanInfo->CurPath);

        if (NULL == pScanInfo->pCurDir)
        {
            return Count;
        }

        //..__wrn("__ScanAllFiles1");
        while (NULL != (pDirent = esFSYS_readdir(pScanInfo->pCurDir)))
        {
            if (pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)
            {
                //..__wrn("__ScanAllFiles2");
                /* ignore "." and ".." sub-directory */
                if (pDirent->d_name[0] == '.')
                {
                    continue;
                }

                /* sub-directory push to stack */
                eLIBs_strcpy(pScanInfo->tempPath, pScanInfo->CurPath);
                eLIBs_strcat(pScanInfo->tempPath, "\\");
                eLIBs_strcat(pScanInfo->tempPath, (char *)pDirent->d_name);

                ScanStackPush(pScanInfo->hStack, pScanInfo->tempPath);
            }
            else
            {
                //..__wrn("__ScanAllFiles3");
                if (EPDK_TRUE == __ValidFileType((const char *)(pDirent->d_name), pScanInfo->ScanTypes, &Format))
                {
                    /* store valid ebook file full path to pScanInfo file list */
                    eLIBs_strcpy(pScanInfo->tempPath, pScanInfo->CurPath);
                    eLIBs_strcat(pScanInfo->tempPath, "\\");
                    eLIBs_strcat(pScanInfo->tempPath, (char *)pDirent->d_name);

                    if (EPDK_OK != __InsertFileEntryToList(pScanInfo->tempPath, Format, pScanInfo))
                    {
                        __wrn("insert file entry to list failed");

                        esFSYS_closedir(pScanInfo->pCurDir);

                        pScanInfo->pCurDir = NULL;
                        return Count;
                    }

                    Count++;

                    if (g_rat_search_cb)
                    {
                        esKRNL_CallBack(g_rat_search_cb, g_rat_search_arg);
                    }
                }
            }
        }

        /* close current open directory, continue to scan sub-directory */
        //..__wrn("__ScanAllFiles6");
        esFSYS_closedir(pScanInfo->pCurDir);

        pScanInfo->pCurDir = NULL;
    }

    //..__wrn("__ScanAllFiles7");
    return Count;
}

int ScanFilesOneTime(ScanInfo_t *pScanInfo)
{
    ES_DIRENT *pDirent;
    int        Format = RAT_MEDIA_TYPE_UNKNOWN;
    int        Count  = 0;

    /* should open directory if necessary */
    if (NULL == pScanInfo->pCurDir)
    {
        if (EPDK_TRUE == ScanStackEmpty(pScanInfo->hStack))
        {
            pScanInfo->CurCnt += Count;
            __wrn("pScanInfo->CurCnt1 ==%d", pScanInfo->CurCnt);
            return Count;
        }

        /* pop directory from stack and save to pScanInfo->CurPath */
        eLIBs_strcpy(pScanInfo->CurPath, ScanStackPop(pScanInfo->hStack));

        pScanInfo->pCurDir  = esFSYS_opendir(pScanInfo->CurPath);

        if (NULL == pScanInfo->pCurDir)
        {
            pScanInfo->CurCnt += Count;
            __wrn("pScanInfo->CurCnt2 ==%d", pScanInfo->CurCnt);
            return Count;
        }
    }

    while (1)
    {
        while (NULL != (pDirent = esFSYS_readdir(pScanInfo->pCurDir)))
        {
            if (pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)
            {
                /* ignore "." and ".." sub-directory */
                if (pDirent->d_name[0] == '.')
                {
                    continue;
                }

                /* sub-directory push to stack */
                __wrn("rat_sprintf");
                rat_sprintf(pScanInfo->tempPath, "%s\\%s", pScanInfo->CurPath, pDirent->d_name);

                ScanStackPush(pScanInfo->hStack, pScanInfo->tempPath);
            }
            else
            {
                __wrn("========scan else");

                if (EPDK_TRUE == __ValidFileType((const char *)(pDirent->d_name), pScanInfo->ScanTypes, &Format))
                {
                    /* store valid ebook file full path to pScanInfo file list */
                    rat_sprintf(pScanInfo->tempPath, "%s\\%s", pScanInfo->CurPath, pDirent->d_name);

                    if (EPDK_OK != __InsertFileEntryToList(pScanInfo->tempPath, Format, pScanInfo))
                    {
                        __wrn("insert file entry to list failed");
                        esFSYS_closedir(pScanInfo->pCurDir);

                        pScanInfo->pCurDir = NULL;
                        pScanInfo->CurCnt += Count;
                        return Count;
                    }

                    /* get one valid ebook file */
                    Count++;

                    if (g_rat_search_cb)
                    {
                        __wrn("scan g_rat_search_cb");
                        esKRNL_CallBack(g_rat_search_cb, g_rat_search_arg);
                    }
                }
            }

            if (Count >= pScanInfo->OnceCnt)
            {
                /* read once work finished */
                pScanInfo->CurCnt += Count;
                __wrn("pScanInfo->CurCnt3 ==%d", pScanInfo->CurCnt);
                return Count;
            }
        }

        /* close current open directory, continue to scan sub-directory */
        esFSYS_closedir(pScanInfo->pCurDir);

        pScanInfo->pCurDir = NULL;

        /* check sub-directory stack empty or not */
        __wrn("check sub-directory stack empty or not");

        if (EPDK_TRUE == ScanStackEmpty(pScanInfo->hStack))
        {
            break;
        }

        /* pop directory from stack and save to pScanInfo->CurPath */
        eLIBs_strcpy(pScanInfo->CurPath, ScanStackPop(pScanInfo->hStack));

        pScanInfo->pCurDir = esFSYS_opendir(pScanInfo->CurPath);

        if (NULL == pScanInfo->pCurDir)
        {
            pScanInfo->CurCnt += Count;
            return Count;
        }
    }

    /* not enough valid ebook file for read */
    pScanInfo->CurCnt += Count;

    __wrn("pScanInfo->CurCnt4 ==%d", pScanInfo->CurCnt);
    return Count;
}

static int __InsertFileEntryToList(const char *filePath, int format, ScanInfo_t *pScanInfo)
{
    ScanFileList_t      *pLastList;
    ScanFileEntry_t     *pEntry;
    __u32               RecLen = 0;

    /* insert file full path to the last file list of pScanInfo */
    pLastList   = pScanInfo->LastList;
    RecLen      = RAT_ALIGN(ENTRY_NAME_OFFSET(pEntry) + eLIBs_strlen(filePath) + 1, sizeof(__u64));

    /* check the last file list have enough space for filePath insert */
    if (RecLen > (pLastList->Alloc - pLastList->Size))
    {
        /* free space not enough, allocate a new file list structure */
        ScanFileList_t  *pNewList;

        pNewList = (ScanFileList_t *)rat_malloc(FILELIST_LEN);

        if (NULL == pNewList)
        {
            __wrn("malloc failed");
            return EPDK_FAIL;
        }

        /* initialize allocated file list */
        pNewList->Prev      = pLastList;
        pLastList->Next     = pNewList;
        pNewList->Next      = NULL;
        pNewList->Alloc     = FILELIST_LEN - LIST_DATA_OFFSET(pNewList);
        pNewList->Size      = 0;
        pNewList->Top       = 0;
        /* insert filePath to the allocated file list */
        pScanInfo->LastList = pNewList;
        pLastList           = pScanInfo->LastList;
    }

    /* store filePath to the end of last file list */
    pEntry          = (ScanFileEntry_t *)(pLastList->Data + pLastList->Size);
    pEntry->RecLen  = RecLen;
    pEntry->Prev    = pLastList->Top;   /* prev entry location */
    pEntry->Format  = format;

    eLIBs_strcpy((char *)(pEntry->Name), filePath);

    /* adjust the last file list management information */
    pLastList->Top    = pLastList->Size;
    pLastList->Size  += RecLen;
    return EPDK_OK;
}

static FmtIdx_t ValidFmtTbl[] =
{
    { ".txt",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_TXT  },
#ifdef CONFIG_SOC_SUN3IW1
    { ".h",     RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_H    },
    { ".c",     RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_C    },
    { ".ini",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_INI  },
#endif
    { ".lrc",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_LRC  },
    { ".srt",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_SRT  },
    { ".ssa",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_SSA  },
    { ".ass",   RAT_MEDIA_TYPE_EBOOK,       EBOOK_SCAN_ASS  },
    //{ ".inf",   RAT_MEDIA_TYPE_EBOOK,         EBOOK_SCAN_INF  },

    { ".AVI",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_AVI  },
    { ".DIVX",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_AVI  },
    { ".PMP",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_PMP  },
    { ".3GP",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_3GP  },
    { ".MP4",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MP4  },
    { ".M4V",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MP4  },
    { ".F4V",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_FLV  },
    { ".MPG",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MPG  },
    { ".MPEG",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MPG  },
    { ".VOB",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_VOB  },
    { ".DAT",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_DAT  },
    { ".RM",    RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_RM   },
    { ".RAM",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_RM   },
    { ".RMVB",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_RMVB },
    { ".FLV",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_FLV  },
    { ".WMV",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_WMV  },
    { ".ASF",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_ASF  },
    { ".MKV",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MKV  },
    { ".PSR",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_PSR  },
    { ".TP",    RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_TP   },
    { ".TRP",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_TS   },
    { ".TS",    RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_TS   },
    { ".M2TS",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_TS   },
    { ".MOV",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_MOV  },
    { ".H264",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_VIDEO_RAW  },
    { ".H265",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_VIDEO_RAW  },
    { ".AVC",   RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_VIDEO_RAW  },
    { ".MP3",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_MP3  },
    { ".MP2",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_MP2  },
    { ".MP1",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_MP1   },
    { ".WMA",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_WMA   },
    { ".OGG",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_OGG },
    { ".APE",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_APE  },
    { ".FLAC",  RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_FLAC  },
    { ".AC3",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_AC3  },
    { ".DTS",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_DTS  },
    { ".RA",    RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_RA  },
    { ".AAC",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_AAC   },
    { ".AMR",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_AMR   },
    { ".ATRC",  RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_ATRC   },
    { ".MID",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_MID   },
    { ".WAV",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_WAV  },
    { ".M4A",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_M4A  },
    { ".OMA",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_OMA   },
    { ".M4R",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_M4R  },
    { ".DRA",   RAT_MEDIA_TYPE_AUDIO,       MUSIC_SCAN_DRA},
    { ".WEBM",  RAT_MEDIA_TYPE_VIDEO,       MOVIE_SCAN_WEBM },
    { ".JFIF",  RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_JFIF  },
    { ".JPEG",  RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_JPEG   },
    { ".PNG",   RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_PNG   },
    { ".BMP",   RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_BMP   },
    { ".JPG",   RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_JPG   },
    { ".GIF",   RAT_MEDIA_TYPE_PIC,         PHOTO_SCAN_GIF  },
    { ".IMG",   RAT_MEDIA_TYPE_FIRMWARE,    IMAGE_SCAN_IMG  },
#if 0
    //{".GBA",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_GBA},
    {".GB",     RAT_MEDIA_TYPE_GAME,       GAME_SCAN_GB},
    {".GBC",    RAT_MEDIA_TYPE_GAME,       GAME_SCAN_GBC},
    {".NES",    RAT_MEDIA_TYPE_GAME,        GAME_SCAN_NES},
    {".NGP",    RAT_MEDIA_TYPE_GAME,       GAME_SCAN_NGP},
    {".PCE",    RAT_MEDIA_TYPE_GAME,       GAME_SCAN_PCE},
    {".SMS",    RAT_MEDIA_TYPE_GAME,        GAME_SCAN_SMS},
    {".GG",     RAT_MEDIA_TYPE_GAME,        GAME_SCAN_GG},
    {".WS",     RAT_MEDIA_TYPE_GAME,        GAME_SCAN_WS},
    {".WSC",    RAT_MEDIA_TYPE_GAME,        GAME_SCAN_WSC},
    //{".ZIP",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_ZIP},
    //{".FBA",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_FBA},
    //{".SMD",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_SMD},
    //{".BIN",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_BIN},
    //{".GEN",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_GEN},
    //{".SMC",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_SMC},
    //{".SFC",  RAT_MEDIA_TYPE_GAME ,        GAME_SCAN_SFC},
#endif
    { NULL,     RAT_MEDIA_TYPE_UNKNOWN,         0         }
};

__bool __ValidFileType(const char *file, __u64 Types, int *Format)
{
    char        *Suffix;
    FmtIdx_t    *pFmt;

    if (NULL == file)
    {
        return EPDK_FALSE;
    }

    /* seek to extended name */
    Suffix = eLIBs_strchrlast((char *)file, '.');

    if (NULL == Suffix)
    {
        return EPDK_FALSE;
    }

    /* look in ebook valid format table */
    pFmt = ValidFmtTbl;

    while (pFmt->Suffix)
    {
        if (eLIBs_stricmp(Suffix, pFmt->Suffix) == 0)
        {
            if ((pFmt->ScanFlag & Types) || 0 == Types)
            {
                *Format = pFmt->Format;
                return EPDK_TRUE;
            }
            else
            {
                return EPDK_FALSE;
            }
        }

        pFmt++;
    }

    if (0 == Types) //RAT_MEDIA_TYPE_ALL
    {
        *Format = RAT_MEDIA_TYPE_UNKNOWN;
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

static void __DestoryScanInfo(ScanInfo_t *pScanInfo)
{
    ScanFileList_t  *pFree;
    ScanFileList_t  *pNextFree;

    /* close current */
    if (pScanInfo->pCurDir)
    {
        esFSYS_closedir(pScanInfo->pCurDir);
    }

    /* free scan stack */
    ScanStackFree(pScanInfo->hStack);
    /* free scaned file lists of pScanInfo*/
    pFree = pScanInfo->FirstList;

    while (pFree)
    {
        pNextFree = pFree->Next;
        rat_free(pFree);
        pFree = pNextFree;
    }

    /* free pScanInfo last */
    rat_free(pScanInfo);
}
