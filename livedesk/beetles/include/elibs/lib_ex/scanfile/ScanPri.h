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

/*
 * tempPath Binary tree storage structure.
 */
typedef struct tempPathTNode
{
    char                       filepath[SCAN_MAX_PATH];
    char                       filename[SCAN_MAX_PATH];
    struct tempPathTNode       *lchild;
    struct tempPathTNode       *rchild;
}tempPathTNode_t,*tempPathTree;

/*************************************************/
//private:
int __ScanMoveCursorToNext(HSCAN hScan, ScanListCursor_t *cursor);
int __ScanMoveCursorToFirst(HSCAN hScan, ScanListCursor_t *cursor);
int __ScanMoveCurosrToPrev(HSCAN hScan, ScanListCursor_t *cursor);
ScanedEntry_t *__ScanGetCurrentCursorData(HSCAN hScan, ScanListCursor_t *cursor);
int       ScanFilesOneTime(ScanInfo_t *pScanInfo);
#endif      /* __SCAN_PRI_H__ */
