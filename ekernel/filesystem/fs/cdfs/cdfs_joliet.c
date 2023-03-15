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
#include "nls.h"
#include "page_pool.h"
#include "fstypedef.h"
#include "fsunaligned.h"
#include "cdfs.h"

/*
 * Convert Unicode 16 to UTF-8 or ASCII.
 */
static int iso_uni16_to_x8(unsigned char *ascii,
                           __be16 *uni,
                           int len,
                           struct nls_table *nls)
{
    __be16 *ip, ch;
    unsigned char *op;

    ip = uni;
    op = ascii;

    while ((ch = get_unaligned(ip)) && len)
    {
        int llen;
        llen = nls->uni2char(be16_to_cpu(ch), op, NLS_MAX_CHARSET_SIZE);
        if (llen > 0)
        {
            op += llen;
        }
        else
        {
            *op++ = '?';
        }
        ip++;

        len--;
    }
    *op = 0;
    return (op - ascii);
}

/* Convert big endian wide character string to utf8 */
static int iso_wcsntombs_be(__u8 *s, const __u8 *pwcs,
                            int inlen, int maxlen)
{
    const __u8 *ip;
    __u8 *op;
    int size;
    __u16 c;

    op = s;
    ip = pwcs;
    while ((*ip || ip[1]) && (maxlen > 0) && (inlen > 0))
    {
        c = (*ip << 8) | ip[1];
        if (c > 0x7f)
        {
            size = utf8_wctomb(op, c, maxlen);
            if (size == -1)
            {
                /* Ignore character and move on */
                maxlen--;
            }
            else
            {
                op += size;
                maxlen -= size;
            }
        }
        else
        {
            *op++ = (__u8) c;
        }
        ip += 2;
        inlen--;
    }
    return (op - s);
}

int get_joliet_filename(struct iso_directory_record *de,
                        unsigned char *outname,
                        struct inode *inode)
{
    unsigned char utf8;
    struct nls_table *nls;
    unsigned char len = 0;

    utf8 = ISOFS_SB(inode->i_sb)->s_utf8;
    nls = ISOFS_SB(inode->i_sb)->s_nls_iocharset;

    if (utf8)
    {
        len = iso_wcsntombs_be(outname, de->name,
                               de->name_len[0] >> 1, PAGE_SIZE);
    }
    else
    {
        len = iso_uni16_to_x8(outname, (__be16 *) de->name,
                              de->name_len[0] >> 1, nls);
    }
    if ((len > 2) && (outname[len - 2] == ';') && (outname[len - 1] == '1'))
    {
        len -= 2;
    }

    /*
     * Windows doesn't like periods at the end of a name,
     * so neither do we
     */
    while (len >= 2 && (outname[len - 1] == '.'))
    {
        len--;
    }

    return len;
}