/**
 * index.c - NTFS index handling.  Originated from the Linux-NTFS project.
 *
 * Copyright (c) 2004-2005 Anton Altaparmakov
 * Copyright (c) 2004-2005 Richard Russon
 * Copyright (c) 2005-2006 Yura Pakhuchiy
 * Copyright (c) 2005-2008 Szabolcs Szakacsits
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <string.h>
#include "attrib.h"
#include "collate.h"
#include "index.h"
#include "mst.h"
#include "dir.h"
#include "bitmap.h"
#include "ntfs.h"
#include "errno.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

#if defined CONFIG_FSYS_NTFS_RW
/**
 * ntfs_index_entry_mark_dirty - mark an index entry dirty
 * @ictx:   ntfs index context describing the index entry
 *
 * Mark the index entry described by the index entry context @ictx dirty.
 *
 * If the index entry is in the index root attribute, simply mark the inode
 * containing the index root attribute dirty.  This ensures the mftrecord, and
 * hence the index root attribute, will be written out to disk later.
 *
 * If the index entry is in an index block belonging to the index allocation
 * attribute, set ib_dirty to TRUE, thus index block will be updated during
 * ntfs_index_ctx_put.
 */
void ntfs_index_entry_mark_dirty(ntfs_index_context *ictx)
{
    if (ictx->is_in_root)
    {
        ntfs_inode_mark_dirty(ictx->actx->ntfs_ino);
    }
    else
    {
        ictx->ib_dirty = TRUE;
    }
}

static s64 ntfs_ib_vcn_to_pos(ntfs_index_context *icx, VCN vcn)
{
    return vcn << icx->vcn_size_bits;
}

static VCN ntfs_ib_pos_to_vcn(ntfs_index_context *icx, s64 pos)
{
    return pos >> icx->vcn_size_bits;
}

static int ntfs_ib_write(ntfs_index_context *icx, INDEX_BLOCK *ib)
{
    s64 ret, vcn = sle64_to_cpu(ib->index_block_vcn);

    fs_log_trace01("vcn: %lld\n", (long long)vcn);

    ret = ntfs_attr_mst_pwrite(icx->ia_na, ntfs_ib_vcn_to_pos(icx, vcn),
                               1, icx->block_size, ib);
    if (ret != 1)
    {
        fs_log_perror("Failed to write index block %lld, inode %llu",
                      (long long)vcn, (unsigned long long)icx->ni->mft_no);
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static int ntfs_icx_ib_write(ntfs_index_context *icx)
{
    if (ntfs_ib_write(icx, icx->ib))
    {
        return STATUS_ERROR;
    }

    icx->ib_dirty = FALSE;

    return STATUS_OK;
}

/**
 * ntfs_index_ctx_get - allocate and initialize a new index context
 * @ni:     ntfs inode with which to initialize the context
 * @name:   name of the which context describes
 * @name_len:   length of the index name
 *
 * Allocate a new index context, initialize it with @ni and return it.
 * Return NULL if allocation failed.
 */
ntfs_index_context *ntfs_index_ctx_get(ntfs_inode *ni,
                                       ntfschar *name, u32 name_len)
{
    ntfs_index_context *icx;

    fs_log_trace01("Entering\n");

    if (!ni)
    {
        fs_err = EINVAL;
        return NULL;
    }
    if (ni->nr_extents == -1)
    {
        ni = ni->base_ni;
    }
    icx = ntfs_calloc(sizeof(ntfs_index_context));
    if (icx)
        *icx = (ntfs_index_context)
    {
        .ni = ni,
        .name = name,
        .name_len = name_len,
    };
    return icx;
}

static void ntfs_index_ctx_free(ntfs_index_context *icx)
{
    fs_log_trace01("Entering\n");

    if (!icx->entry)
    {
        return;
    }

    if (icx->actx)
    {
        ntfs_attr_put_search_ctx(icx->actx);
    }

    if (!icx->is_in_root)
    {
        if (icx->ib_dirty)
        {
            /* FIXME: Error handling!!! */
            ntfs_ib_write(icx, icx->ib);
        }
        free(icx->ib);
    }

    ntfs_attr_close(icx->ia_na);
}

/**
 * ntfs_index_ctx_put - release an index context
 * @icx:    index context to free
 *
 * Release the index context @icx, releasing all associated resources.
 */
void ntfs_index_ctx_put(ntfs_index_context *icx)
{
    ntfs_index_ctx_free(icx);
    free(icx);
}

static VCN *ntfs_ie_get_vcn_addr(INDEX_ENTRY *ie)
{
    return (VCN *)((u8 *)ie + le16_to_cpu(ie->length) - sizeof(VCN));
}

/**
 *  Get the subnode vcn to which the index entry refers.
 */
VCN ntfs_ie_get_vcn(INDEX_ENTRY *ie)
{
    return sle64_to_cpup(ntfs_ie_get_vcn_addr(ie));
}

static INDEX_ENTRY *ntfs_ie_get_first(INDEX_HEADER *ih)
{
    return (INDEX_ENTRY *)((u8 *)ih + le32_to_cpu(ih->entries_offset));
}

static INDEX_ENTRY *ntfs_ie_get_next(INDEX_ENTRY *ie)
{
    return (INDEX_ENTRY *)((char *)ie + le16_to_cpu(ie->length));
}

static u8 *ntfs_ie_get_end(INDEX_HEADER *ih)
{
    /* FIXME: check if it isn't overflowing the index block size */
    return (u8 *)ih + le32_to_cpu(ih->index_length);
}

static int ntfs_ie_end(INDEX_ENTRY *ie)
{
    return ie->ie_flags & INDEX_ENTRY_END || !ie->length;
}

/**
 *  Find the last entry in the index block
 */
static INDEX_ENTRY *ntfs_ie_get_last(INDEX_ENTRY *ie, char *ies_end)
{
    fs_log_trace01("Entering\n");

    while ((char *)ie < ies_end && !ntfs_ie_end(ie))
    {
        ie = ntfs_ie_get_next(ie);
    }

    return ie;
}

static INDEX_ENTRY *ntfs_ie_get_by_pos(INDEX_HEADER *ih, int pos)
{
    INDEX_ENTRY *ie;

    fs_log_trace01("pos: %d\n", pos);

    ie = ntfs_ie_get_first(ih);

    while (pos-- > 0)
    {
        ie = ntfs_ie_get_next(ie);
    }

    return ie;
}

static INDEX_ENTRY *ntfs_ie_prev(INDEX_HEADER *ih, INDEX_ENTRY *ie)
{
    INDEX_ENTRY *ie_prev = NULL;
    INDEX_ENTRY *tmp;

    fs_log_trace01("Entering\n");

    tmp = ntfs_ie_get_first(ih);

    while (tmp != ie)
    {
        ie_prev = tmp;
        tmp = ntfs_ie_get_next(tmp);
    }

    return ie_prev;
}

static int ntfs_ih_numof_entries(INDEX_HEADER *ih)
{
    int n;
    INDEX_ENTRY *ie;
    u8 *end;

    fs_log_trace01("Entering\n");

    end = ntfs_ie_get_end(ih);
    ie = ntfs_ie_get_first(ih);
    for (n = 0; !ntfs_ie_end(ie) && (u8 *)ie < end; n++)
    {
        ie = ntfs_ie_get_next(ie);
    }
    return n;
}

static int ntfs_ih_one_entry(INDEX_HEADER *ih)
{
    return (ntfs_ih_numof_entries(ih) == 1);
}

static int ntfs_ih_zero_entry(INDEX_HEADER *ih)
{
    return (ntfs_ih_numof_entries(ih) == 0);
}

static void ntfs_ie_delete(INDEX_HEADER *ih, INDEX_ENTRY *ie)
{
    u32 new_size;

    fs_log_trace01("Entering\n");

    new_size = le32_to_cpu(ih->index_length) - le16_to_cpu(ie->length);
    ih->index_length = cpu_to_le32(new_size);
    memmove(ie, (u8 *)ie + le16_to_cpu(ie->length),
            new_size - ((u8 *)ie - (u8 *)ih));
}

static void ntfs_ie_set_vcn(INDEX_ENTRY *ie, VCN vcn)
{
    *ntfs_ie_get_vcn_addr(ie) = cpu_to_le64(vcn);
}

/**
 *  Insert @ie index entry at @pos entry. Used @ih values should be ok already.
 */
static void ntfs_ie_insert(INDEX_HEADER *ih, INDEX_ENTRY *ie, INDEX_ENTRY *pos)
{
    int ie_size = le16_to_cpu(ie->length);

    fs_log_trace01("Entering\n");

    ih->index_length = cpu_to_le32(le32_to_cpu(ih->index_length) + ie_size);
    memmove((u8 *)pos + ie_size, pos,
            le32_to_cpu(ih->index_length) - ((u8 *)pos - (u8 *)ih) - ie_size);
    memcpy(pos, ie, ie_size);
}

static INDEX_ENTRY *ntfs_ie_dup(INDEX_ENTRY *ie)
{
    INDEX_ENTRY *dup;

    fs_log_trace01("Entering\n");

    dup = ntfs_malloc(le16_to_cpu(ie->length));
    if (dup)
    {
        memcpy(dup, ie, le16_to_cpu(ie->length));
    }

    return dup;
}

static INDEX_ENTRY *ntfs_ie_dup_novcn(INDEX_ENTRY *ie)
{
    INDEX_ENTRY *dup;
    int size = le16_to_cpu(ie->length);

    fs_log_trace01("Entering\n");

    if (ie->ie_flags & INDEX_ENTRY_NODE)
    {
        size -= sizeof(VCN);
    }

    dup = ntfs_malloc(size);
    if (dup)
    {
        memcpy(dup, ie, size);
        dup->ie_flags &= ~INDEX_ENTRY_NODE;
        dup->length = cpu_to_le16(size);
    }
    return dup;
}

static int ntfs_ia_check(ntfs_index_context *icx, INDEX_BLOCK *ib, VCN vcn)
{
    u32 ib_size = (unsigned)le32_to_cpu(ib->index.allocated_size) + 0x18;

    fs_log_trace01("Entering\n");

    if (!ntfs_is_indx_record(ib->magic))
    {

        fs_log_error("Corrupt index block signature: vcn %lld inode "
                     "%llu\n", (long long)vcn,
                     (unsigned long long)icx->ni->mft_no);
        return -1;
    }

    if (sle64_to_cpu(ib->index_block_vcn) != vcn)
    {

        fs_log_error("Corrupt index block: VCN (%lld) is different "
                     "from expected VCN (%lld) in inode %llu\n",
                     (long long)sle64_to_cpu(ib->index_block_vcn),
                     (long long)vcn,
                     (unsigned long long)icx->ni->mft_no);
        return -1;
    }

    if (ib_size != icx->block_size)
    {

        fs_log_error("Corrupt index block : VCN (%lld) of inode %llu "
                     "has a size (%u) differing from the index "
                     "specified size (%u)\n", (long long)vcn,
                     (unsigned long long)icx->ni->mft_no, ib_size,
                     icx->block_size);
        return -1;
    }
    return 0;
}

static INDEX_ROOT *ntfs_ir_lookup(ntfs_inode *ni, ntfschar *name,
                                  u32 name_len, ntfs_attr_search_ctx **ctx)
{
    ATTR_RECORD *a;
    INDEX_ROOT *ir = NULL;

    fs_log_trace01("Entering\n");

    *ctx = ntfs_attr_get_search_ctx(ni, NULL);
    if (!*ctx)
    {
        return NULL;
    }

    if (ntfs_attr_lookup(AT_INDEX_ROOT, name, name_len, CASE_SENSITIVE,
                         0, NULL, 0, *ctx))
    {
        fs_log_perror("Failed to lookup $INDEX_ROOT");
        goto err_out;
    }

    a = (*ctx)->attr;
    if (a->non_resident)
    {
        fs_err = EINVAL;
        fs_log_perror("Non-resident $INDEX_ROOT detected");
        goto err_out;
    }

    ir = (INDEX_ROOT *)((char *)a + le16_to_cpu(a->value_offset));
err_out:
    if (!ir)
    {
        ntfs_attr_put_search_ctx(*ctx);
        *ctx = NULL;
    }
    return ir;
}

static INDEX_ROOT *ntfs_ir_lookup2(ntfs_inode *ni, ntfschar *name, u32 len)
{
    ntfs_attr_search_ctx *ctx;
    INDEX_ROOT *ir;

    ir = ntfs_ir_lookup(ni, name, len, &ctx);
    if (ir)
    {
        ntfs_attr_put_search_ctx(ctx);
    }
    return ir;
}

/**
 * Find a key in the index block.
 *
 * Return values:
 *   STATUS_OK with fs_err set to ESUCCESS if we know for sure that the
 *             entry exists and @ie_out points to this entry.
 *   STATUS_NOT_FOUND with fs_err set to ENOENT if we know for sure the
 *                    entry doesn't exist and @ie_out is the insertion point.
 *   STATUS_KEEP_SEARCHING if we can't answer the above question and
 *                         @vcn will contain the node index block.
 *   STATUS_ERROR with fs_err set if on unexpected error during lookup.
 */
static int ntfs_ie_lookup(const void *key, const int key_len,
                          ntfs_index_context *icx, INDEX_HEADER *ih,
                          VCN *vcn, INDEX_ENTRY **ie_out)
{
    INDEX_ENTRY *ie;
    u8 *index_end;
    int rc, item = 0;

    fs_log_trace01("Entering\n");

    index_end = ntfs_ie_get_end(ih);

    /*
     * Loop until we exceed valid memory (corruption case) or until we
     * reach the last entry.
     */
    for (ie = ntfs_ie_get_first(ih); ; ie = ntfs_ie_get_next(ie))
    {
        /* Bounds checks. */
        if ((u8 *)ie + sizeof(INDEX_ENTRY_HEADER) > index_end ||
            (u8 *)ie + le16_to_cpu(ie->length) > index_end)
        {
            fs_err = ERANGE;
            fs_log_error("Index entry out of bounds in inode "
                         "%llu.\n",
                         (unsigned long long)icx->ni->mft_no);
            return STATUS_ERROR;
        }
        /*
         * The last entry cannot contain a key.  It can however contain
         * a pointer to a child node in the B+tree so we just break out.
         */
        if (ntfs_ie_end(ie))
        {
            break;
        }
        /*
         * Not a perfect match, need to do full blown collation so we
         * know which way in the B+tree we have to go.
         */
        rc = ntfs_collate(icx->ni->vol, icx->cr, key, key_len, &ie->key,
                          le16_to_cpu(ie->key_length));
        if (rc == NTFS_COLLATION_ERROR)
        {
            fs_log_error("Collation error. Perhaps a filename "
                         "contains invalid characters?\n");
            fs_err = ERANGE;
            return STATUS_ERROR;
        }
        /*
         * If @key collates before the key of the current entry, there
         * is definitely no such key in this index but we might need to
         * descend into the B+tree so we just break out of the loop.
         */
        if (rc == -1)
        {
            break;
        }

        if (!rc)
        {
            *ie_out = ie;
            fs_err = 0;
            icx->parent_pos[icx->pindex] = item;
            return STATUS_OK;
        }

        item++;
    }
    /*
     * We have finished with this index block without success. Check for the
     * presence of a child node and if not present return with fs_err ENOENT,
     * otherwise we will keep searching in another index block.
     */
    if (!(ie->ie_flags & INDEX_ENTRY_NODE))
    {
        fs_log_debug("Index entry wasn't found.\n");
        *ie_out = ie;
        fs_err = ENOENT;
        return STATUS_NOT_FOUND;
    }

    /* Get the starting vcn of the index_block holding the child node. */
    *vcn = ntfs_ie_get_vcn(ie);
    if (*vcn < 0)
    {
        fs_err = EINVAL;
        fs_log_perror("Negative vcn in inode %llu\n",
                      (unsigned long long)icx->ni->mft_no);
        return STATUS_ERROR;
    }

    fs_log_trace01("Parent entry number %d\n", item);
    icx->parent_pos[icx->pindex] = item;

    return STATUS_KEEP_SEARCHING;
}

static ntfs_attr *ntfs_ia_open(ntfs_index_context *icx, ntfs_inode *ni)
{
    ntfs_attr *na;

    na = ntfs_attr_open(ni, AT_INDEX_ALLOCATION, icx->name, icx->name_len);
    if (!na)
    {
        fs_log_perror("Failed to open index allocation of inode "
                      "%llu", (unsigned long long)ni->mft_no);
        return NULL;
    }

    return na;
}

static int ntfs_ib_read(ntfs_index_context *icx, VCN vcn, INDEX_BLOCK *dst)
{
    s64 pos, ret;

    fs_log_trace01("vcn: %lld\n", (long long)vcn);

    pos = ntfs_ib_vcn_to_pos(icx, vcn);

    ret = ntfs_attr_mst_pread(icx->ia_na, pos, 1, icx->block_size, (u8 *)dst);
    if (ret != 1)
    {
        if (ret == -1)
        {
            fs_log_perror("Failed to read index block");
        }
        else
            fs_log_error("Failed to read full index block at "
                         "%lld\n", (long long)pos);
        return -1;
    }

    if (ntfs_ia_check(icx, dst, vcn))
    {
        return -1;
    }

    return 0;
}

static int ntfs_icx_parent_inc(ntfs_index_context *icx)
{
    icx->pindex++;
    if (icx->pindex >= MAX_PARENT_VCN)
    {
        fs_err = EOPNOTSUPP;
        fs_log_perror("Index is over %d level deep", MAX_PARENT_VCN);
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

static int ntfs_icx_parent_dec(ntfs_index_context *icx)
{
    icx->pindex--;
    if (icx->pindex < 0)
    {
        fs_err = EINVAL;
        fs_log_perror("Corrupt index pointer (%d)", icx->pindex);
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

/**
 * ntfs_index_lookup - find a key in an index and return its index entry
 * @key:    [IN] key for which to search in the index
 * @key_len:    [IN] length of @key in bytes
 * @icx:    [IN/OUT] context describing the index and the returned entry
 *
 * Before calling ntfs_index_lookup(), @icx must have been obtained from a
 * call to ntfs_index_ctx_get().
 *
 * Look for the @key in the index specified by the index lookup context @icx.
 * ntfs_index_lookup() walks the contents of the index looking for the @key.
 *
 * If the @key is found in the index, 0 is returned and @icx is setup to
 * describe the index entry containing the matching @key.  @icx->entry is the
 * index entry and @icx->data and @icx->data_len are the index entry data and
 * its length in bytes, respectively.
 *
 * If the @key is not found in the index, -1 is returned, fs_err = ENOENT and
 * @icx is setup to describe the index entry whose key collates immediately
 * after the search @key, i.e. this is the position in the index at which
 * an index entry with a key of @key would need to be inserted.
 *
 * If an error occurs return -1, set fs_err to error code and @icx is left
 * untouched.
 *
 * When finished with the entry and its data, call ntfs_index_ctx_put() to free
 * the context and other associated resources.
 *
 * If the index entry was modified, call ntfs_index_entry_mark_dirty() before
 * the call to ntfs_index_ctx_put() to ensure that the changes are written
 * to disk.
 */
int ntfs_index_lookup(const void *key, const int key_len, ntfs_index_context *icx)
{
    VCN old_vcn, vcn;
    ntfs_inode *ni = icx->ni;
    INDEX_ROOT *ir;
    INDEX_ENTRY *ie;
    INDEX_BLOCK *ib = NULL;
    int ret, err = 0;

    fs_log_trace01("Entering\n");

    if (!key || key_len <= 0)
    {
        fs_err = EINVAL;
        fs_log_perror("key: %p  key_len: %d", key, key_len);
        return -1;
    }

    ir = ntfs_ir_lookup(ni, icx->name, icx->name_len, &icx->actx);
    if (!ir)
    {
        if (fs_err == ENOENT)
        {
            fs_err = EIO;
        }
        return -1;
    }

    icx->block_size = le32_to_cpu(ir->index_block_size);
    if (icx->block_size < NTFS_BLOCK_SIZE)
    {
        fs_err = EINVAL;
        fs_log_perror("Index block size (%d) is smaller than the "
                      "sector size (%d)", icx->block_size, NTFS_BLOCK_SIZE);
        goto err_out;
    }

    if (ni->vol->cluster_size <= icx->block_size)
    {
        icx->vcn_size_bits = ni->vol->cluster_size_bits;
    }
    else
    {
        icx->vcn_size_bits = ni->vol->sector_size_bits;
    }

    icx->cr = ir->collation_rule;
    if (!ntfs_is_collation_rule_supported(icx->cr))
    {
        err = fs_err = EOPNOTSUPP;
        fs_log_perror("Unknown collation rule 0x%x",
                      (unsigned)le32_to_cpu(icx->cr));
        goto err_out;
    }

    old_vcn = VCN_INDEX_ROOT_PARENT;
    /*
     * FIXME: check for both ir and ib that the first index entry is
     * within the index block.
     */
    ret = ntfs_ie_lookup(key, key_len, icx, &ir->index, &vcn, &ie);
    if (ret == STATUS_ERROR)
    {
        err = fs_err;
        goto err_out;
    }

    icx->ir = ir;

    if (ret != STATUS_KEEP_SEARCHING)
    {
        /* STATUS_OK or STATUS_NOT_FOUND */
        err = fs_err;
        icx->is_in_root = TRUE;
        icx->parent_vcn[icx->pindex] = old_vcn;
        goto done;
    }

    /* Child node present, descend into it. */

    icx->ia_na = ntfs_ia_open(icx, ni);
    if (!icx->ia_na)
    {
        goto err_out;
    }

    ib = ntfs_malloc(icx->block_size);
    if (!ib)
    {
        err = fs_err;
        goto err_out;
    }

descend_into_child_node:

    icx->parent_vcn[icx->pindex] = old_vcn;
    if (ntfs_icx_parent_inc(icx))
    {
        err = fs_err;
        goto err_out;
    }
    old_vcn = vcn;

    fs_log_debug("Descend into node with VCN %lld\n", (long long)vcn);

    if (ntfs_ib_read(icx, vcn, ib))
    {
        goto err_out;
    }

    ret = ntfs_ie_lookup(key, key_len, icx, &ib->index, &vcn, &ie);
    if (ret != STATUS_KEEP_SEARCHING)
    {
        err = fs_err;
        if (ret == STATUS_ERROR)
        {
            goto err_out;
        }

        /* STATUS_OK or STATUS_NOT_FOUND */
        icx->is_in_root = FALSE;
        icx->ib = ib;
        icx->parent_vcn[icx->pindex] = vcn;
        goto done;
    }

    if ((ib->index.ih_flags & NODE_MASK) == LEAF_NODE)
    {
        fs_log_error("Index entry with child node found in a leaf "
                     "node in inode 0x%llx.\n",
                     (unsigned long long)ni->mft_no);
        goto err_out;
    }

    goto descend_into_child_node;
err_out:
    free(ib);
    if (!err)
    {
        err = EIO;
    }
    fs_err = err;
    return -1;
done:
    icx->entry = ie;
    icx->data = (u8 *)ie + offsetof(INDEX_ENTRY, key);
    icx->data_len = le16_to_cpu(ie->key_length);
    fs_log_trace01("Done.\n");
    if (err)
    {
        fs_err = err;
        return -1;
    }
    return 0;

}

static INDEX_BLOCK *ntfs_ib_alloc(VCN ib_vcn, u32 ib_size,
                                  INDEX_HEADER_FLAGS node_type)
{
    INDEX_BLOCK *ib;
    int ih_size = sizeof(INDEX_HEADER);

    fs_log_trace01("ib_vcn: %lld ib_size: %u\n", (long long)ib_vcn, ib_size);

    ib = ntfs_calloc(ib_size);
    if (!ib)
    {
        return NULL;
    }

    ib->magic = magic_INDX;
    ib->usa_ofs = cpu_to_le16(sizeof(INDEX_BLOCK));
    ib->usa_count = cpu_to_le16(ib_size / NTFS_BLOCK_SIZE + 1);
    /* Set USN to 1 */
    *(u16 *)((char *)ib + le16_to_cpu(ib->usa_ofs)) = cpu_to_le16(1);
    ib->lsn = cpu_to_le64(0);

    ib->index_block_vcn = cpu_to_sle64(ib_vcn);

    ib->index.entries_offset = cpu_to_le32((ih_size +
                                            le16_to_cpu(ib->usa_count) * 2 + 7) & ~7);
    ib->index.index_length = 0;
    ib->index.allocated_size = cpu_to_le32(ib_size -
                                           (sizeof(INDEX_BLOCK) - ih_size));
    ib->index.ih_flags = node_type;

    return ib;
}

/**
 *  Find the median by going through all the entries
 */
static INDEX_ENTRY *ntfs_ie_get_median(INDEX_HEADER *ih)
{
    INDEX_ENTRY *ie, *ie_start;
    u8 *ie_end;
    int i = 0, median;

    fs_log_trace01("Entering\n");

    ie = ie_start = ntfs_ie_get_first(ih);
    ie_end   = (u8 *)ntfs_ie_get_end(ih);

    while ((u8 *)ie < ie_end && !ntfs_ie_end(ie))
    {
        ie = ntfs_ie_get_next(ie);
        i++;
    }
    /*
     * NOTE: this could be also the entry at the half of the index block.
     */
    median = i / 2 - 1;

    fs_log_trace01("Entries: %d  median: %d\n", i, median);

    for (i = 0, ie = ie_start; i <= median; i++)
    {
        ie = ntfs_ie_get_next(ie);
    }

    return ie;
}

static s64 ntfs_ibm_vcn_to_pos(ntfs_index_context *icx, VCN vcn)
{
    return ntfs_ib_vcn_to_pos(icx, vcn) / icx->block_size;
}

static s64 ntfs_ibm_pos_to_vcn(ntfs_index_context *icx, s64 pos)
{
    return ntfs_ib_pos_to_vcn(icx, pos * icx->block_size);
}

static int ntfs_ibm_add(ntfs_index_context *icx)
{
    u8 bmp[8];

    fs_log_trace01("Entering\n");

    if (ntfs_attr_exist(icx->ni, AT_BITMAP, icx->name, icx->name_len))
    {
        return STATUS_OK;
    }
    /*
     * AT_BITMAP must be at least 8 bytes.
     */
    memset(bmp, 0, sizeof(bmp));
    if (ntfs_attr_add(icx->ni, AT_BITMAP, icx->name, icx->name_len,
                      bmp, sizeof(bmp)))
    {
        fs_log_perror("Failed to add AT_BITMAP");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static int ntfs_ibm_modify(ntfs_index_context *icx, VCN vcn, int set)
{
    u8 byte;
    s64 pos = ntfs_ibm_vcn_to_pos(icx, vcn);
    u32 bpos = pos / 8;
    u32 bit = 1 << (pos % 8);
    ntfs_attr *na;
    int ret = STATUS_ERROR;

    fs_log_trace01("%s vcn: %lld\n", set ? "set" : "clear", (long long)vcn);

    na = ntfs_attr_open(icx->ni, AT_BITMAP,  icx->name, icx->name_len);
    if (!na)
    {
        fs_log_perror("Failed to open $BITMAP attribute");
        return -1;
    }

    if (set)
    {
        if (na->data_size < bpos + 1)
        {
            if (ntfs_attr_truncate(na, (na->data_size + 8) & ~7))
            {
                fs_log_perror("Failed to truncate AT_BITMAP");
                goto err_na;
            }
        }
    }

    if (ntfs_attr_pread(na, bpos, 1, &byte) != 1)
    {
        fs_log_perror("Failed to read $BITMAP");
        goto err_na;
    }

    if (set)
    {
        byte |= bit;
    }
    else
    {
        byte &= ~bit;
    }

    if (ntfs_attr_pwrite(na, bpos, 1, &byte) != 1)
    {
        fs_log_perror("Failed to write $Bitmap");
        goto err_na;
    }

    ret = STATUS_OK;
err_na:
    ntfs_attr_close(na);
    return ret;
}


static int ntfs_ibm_set(ntfs_index_context *icx, VCN vcn)
{
    return ntfs_ibm_modify(icx, vcn, 1);
}

static int ntfs_ibm_clear(ntfs_index_context *icx, VCN vcn)
{
    return ntfs_ibm_modify(icx, vcn, 0);
}

static VCN ntfs_ibm_get_free(ntfs_index_context *icx)
{
    u8 *bm;
    int bit;
    s64 vcn, byte, size;

    fs_log_trace01("Entering\n");

    bm = ntfs_attr_readall(icx->ni, AT_BITMAP,  icx->name, icx->name_len,
                           &size);
    if (!bm)
    {
        return (VCN) - 1;
    }

    for (byte = 0; byte < size; byte++)
    {

        if (bm[byte] == 255)
        {
            continue;
        }

        for (bit = 0; bit < 8; bit++)
        {
            if (!(bm[byte] & (1 << bit)))
            {
                vcn = ntfs_ibm_pos_to_vcn(icx, byte * 8 + bit);
                goto out;
            }
        }
    }

    vcn = ntfs_ibm_pos_to_vcn(icx, size * 8);
out:
    fs_log_trace01("allocated vcn: %lld\n", (long long)vcn);

    if (ntfs_ibm_set(icx, vcn))
    {
        vcn = (VCN) - 1;
    }

    free(bm);
    return vcn;
}

static INDEX_BLOCK *ntfs_ir_to_ib(INDEX_ROOT *ir, VCN ib_vcn)
{
    INDEX_BLOCK *ib;
    INDEX_ENTRY *ie_last;
    char *ies_start, *ies_end;
    int i;

    fs_log_trace01("Entering\n");

    ib = ntfs_ib_alloc(ib_vcn, le32_to_cpu(ir->index_block_size), LEAF_NODE);
    if (!ib)
    {
        return NULL;
    }

    ies_start = (char *)ntfs_ie_get_first(&ir->index);
    ies_end   = (char *)ntfs_ie_get_end(&ir->index);
    ie_last   = ntfs_ie_get_last((INDEX_ENTRY *)ies_start, ies_end);
    /*
     * Copy all entries, including the termination entry
     * as well, which can never have any data.
     */
    i = (char *)ie_last - ies_start + le16_to_cpu(ie_last->length);
    memcpy(ntfs_ie_get_first(&ib->index), ies_start, i);

    ib->index.ih_flags = ir->index.ih_flags;
    ib->index.index_length = cpu_to_le32(i +
                                         le32_to_cpu(ib->index.entries_offset));
    return ib;
}

static void ntfs_ir_nill(INDEX_ROOT *ir)
{
    INDEX_ENTRY *ie_last;
    char *ies_start, *ies_end;

    fs_log_trace01("Entering\n");
    /*
     * TODO: This function could be much simpler.
     */
    ies_start = (char *)ntfs_ie_get_first(&ir->index);
    ies_end   = (char *)ntfs_ie_get_end(&ir->index);
    ie_last   = ntfs_ie_get_last((INDEX_ENTRY *)ies_start, ies_end);
    /*
     * Move the index root termination entry forward
     */
    if ((char *)ie_last > ies_start)
    {
        memmove(ies_start, (char *)ie_last, le16_to_cpu(ie_last->length));
        ie_last = (INDEX_ENTRY *)ies_start;
    }
}

static int ntfs_ib_copy_tail(ntfs_index_context *icx, INDEX_BLOCK *src,
                             INDEX_ENTRY *median, VCN new_vcn)
{
    u8 *ies_end;
    INDEX_ENTRY *ie_head;       /* first entry after the median */
    int tail_size, ret;
    INDEX_BLOCK *dst;

    fs_log_trace01("Entering\n");

    dst = ntfs_ib_alloc(new_vcn, icx->block_size,
                        src->index.ih_flags & NODE_MASK);
    if (!dst)
    {
        return STATUS_ERROR;
    }

    ie_head = ntfs_ie_get_next(median);

    ies_end = (u8 *)ntfs_ie_get_end(&src->index);
    tail_size = ies_end - (u8 *)ie_head;
    memcpy(ntfs_ie_get_first(&dst->index), ie_head, tail_size);

    dst->index.index_length = cpu_to_le32(tail_size +
                                          le32_to_cpu(dst->index.entries_offset));
    ret = ntfs_ib_write(icx, dst);

    free(dst);
    return ret;
}

static int ntfs_ib_cut_tail(ntfs_index_context *icx, INDEX_BLOCK *ib,
                            INDEX_ENTRY *ie)
{
    char *ies_start, *ies_end;
    INDEX_ENTRY *ie_last;

    fs_log_trace01("Entering\n");

    ies_start = (char *)ntfs_ie_get_first(&ib->index);
    ies_end   = (char *)ntfs_ie_get_end(&ib->index);

    ie_last   = ntfs_ie_get_last((INDEX_ENTRY *)ies_start, ies_end);
    if (ie_last->ie_flags & INDEX_ENTRY_NODE)
    {
        ntfs_ie_set_vcn(ie_last, ntfs_ie_get_vcn(ie));
    }

    memcpy(ie, ie_last, le16_to_cpu(ie_last->length));

    ib->index.index_length = cpu_to_le32(((char *)ie - ies_start) +
                                         le16_to_cpu(ie->length) + le32_to_cpu(ib->index.entries_offset));

    if (ntfs_ib_write(icx, ib))
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static int ntfs_ia_add(ntfs_index_context *icx)
{
    fs_log_trace01("Entering\n");

    if (ntfs_ibm_add(icx))
    {
        return -1;
    }

    if (!ntfs_attr_exist(icx->ni, AT_INDEX_ALLOCATION, icx->name, icx->name_len))
    {

        if (ntfs_attr_add(icx->ni, AT_INDEX_ALLOCATION, icx->name,
                          icx->name_len, NULL, 0))
        {
            fs_log_perror("Failed to add AT_INDEX_ALLOCATION");
            return -1;
        }
    }

    icx->ia_na = ntfs_ia_open(icx, icx->ni);
    if (!icx->ia_na)
    {
        return -1;
    }

    return 0;
}

static int ntfs_ir_reparent(ntfs_index_context *icx)
{
    ntfs_attr_search_ctx *ctx = NULL;
    INDEX_ROOT *ir;
    INDEX_ENTRY *ie;
    INDEX_BLOCK *ib = NULL;
    VCN new_ib_vcn;
    int ret = STATUS_ERROR;

    fs_log_trace01("Entering\n");

    ir = ntfs_ir_lookup2(icx->ni, icx->name, icx->name_len);
    if (!ir)
    {
        goto out;
    }

    if ((ir->index.ih_flags & NODE_MASK) == SMALL_INDEX)
        if (ntfs_ia_add(icx))
        {
            goto out;
        }

    new_ib_vcn = ntfs_ibm_get_free(icx);
    if (new_ib_vcn == -1)
    {
        goto out;
    }

    ir = ntfs_ir_lookup2(icx->ni, icx->name, icx->name_len);
    if (!ir)
    {
        goto clear_bmp;
    }

    ib = ntfs_ir_to_ib(ir, new_ib_vcn);
    if (ib == NULL)
    {
        fs_log_perror("Failed to move index root to index block");
        goto clear_bmp;
    }

    if (ntfs_ib_write(icx, ib))
    {
        goto clear_bmp;
    }

    ir = ntfs_ir_lookup(icx->ni, icx->name, icx->name_len, &ctx);
    if (!ir)
    {
        goto clear_bmp;
    }

    ntfs_ir_nill(ir);

    ie = ntfs_ie_get_first(&ir->index);
    ie->ie_flags |= INDEX_ENTRY_NODE;
    ie->length = cpu_to_le16(sizeof(INDEX_ENTRY_HEADER) + sizeof(VCN));

    ir->index.ih_flags = LARGE_INDEX;
    ir->index.index_length = cpu_to_le32(le32_to_cpu(ir->index.entries_offset)
                                         + le16_to_cpu(ie->length));
    ir->index.allocated_size = ir->index.index_length;

    if (ntfs_resident_attr_value_resize(ctx->mrec, ctx->attr,
                                        sizeof(INDEX_ROOT) - sizeof(INDEX_HEADER) +
                                        le32_to_cpu(ir->index.allocated_size)))
        /* FIXME: revert index root */
    {
        goto clear_bmp;
    }
    /*
     *  FIXME: do it earlier if we have enough space in IR (should always),
     *  so in error case we wouldn't lose the IB.
     */
    ntfs_ie_set_vcn(ie, new_ib_vcn);

    ret = STATUS_OK;
err_out:
    free(ib);
    ntfs_attr_put_search_ctx(ctx);
out:
    return ret;
clear_bmp:
    ntfs_ibm_clear(icx, new_ib_vcn);
    goto err_out;
}

/**
 * ntfs_ir_truncate - Truncate index root attribute
 *
 * Returns STATUS_OK, STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT or STATUS_ERROR.
 */
static int ntfs_ir_truncate(ntfs_index_context *icx, int data_size)
{
    ntfs_attr *na;
    int ret;

    fs_log_trace01("Entering\n");

    na = ntfs_attr_open(icx->ni, AT_INDEX_ROOT, icx->name, icx->name_len);
    if (!na)
    {
        fs_log_perror("Failed to open INDEX_ROOT");
        return STATUS_ERROR;
    }
    /*
     *  INDEX_ROOT must be resident and its entries can be moved to
     *  INDEX_BLOCK, so ENOSPC isn't a real error.
     */
    ret = ntfs_attr_truncate(na, data_size + offsetof(INDEX_ROOT, index));
    if (ret == STATUS_OK)
    {

        icx->ir = ntfs_ir_lookup2(icx->ni, icx->name, icx->name_len);
        if (!icx->ir)
        {
            return STATUS_ERROR;
        }

        icx->ir->index.allocated_size = cpu_to_le32(data_size);

    }
    else if (ret == STATUS_ERROR)
    {
        fs_log_perror("Failed to truncate INDEX_ROOT");
    }

    ntfs_attr_close(na);
    return ret;
}

/**
 * ntfs_ir_make_space - Make more space for the index root attribute
 *
 * On success return STATUS_OK or STATUS_KEEP_SEARCHING.
 * On error return STATUS_ERROR.
 */
static int ntfs_ir_make_space(ntfs_index_context *icx, int data_size)
{
    int ret;

    fs_log_trace01("Entering\n");

    ret = ntfs_ir_truncate(icx, data_size);
    if (ret == STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT)
    {

        ret = ntfs_ir_reparent(icx);
        if (ret == STATUS_OK)
        {
            ret = STATUS_KEEP_SEARCHING;
        }
        else
        {
            fs_log_perror("Failed to nodify INDEX_ROOT");
        }
    }

    return ret;
}

/*
 * NOTE: 'ie' must be a copy of a real index entry.
 */
static int ntfs_ie_add_vcn(INDEX_ENTRY **ie)
{
    INDEX_ENTRY *p, *old = *ie;

    old->length = cpu_to_le16(le16_to_cpu(old->length) + sizeof(VCN));
    p = realloc(old, le16_to_cpu(old->length));
    if (!p)
    {
        return STATUS_ERROR;
    }

    p->ie_flags |= INDEX_ENTRY_NODE;
    *ie = p;

    return STATUS_OK;
}

static int ntfs_ih_insert(INDEX_HEADER *ih, INDEX_ENTRY *orig_ie, VCN new_vcn,
                          int pos)
{
    INDEX_ENTRY *ie_node, *ie;
    int ret = STATUS_ERROR;
    VCN old_vcn;

    fs_log_trace01("Entering\n");

    ie = ntfs_ie_dup(orig_ie);
    if (!ie)
    {
        return STATUS_ERROR;
    }

    if (!(ie->ie_flags & INDEX_ENTRY_NODE))
        if (ntfs_ie_add_vcn(&ie))
        {
            goto out;
        }

    ie_node = ntfs_ie_get_by_pos(ih, pos);
    old_vcn = ntfs_ie_get_vcn(ie_node);
    ntfs_ie_set_vcn(ie_node, new_vcn);

    ntfs_ie_insert(ih, ie, ie_node);
    ntfs_ie_set_vcn(ie_node, old_vcn);
    ret = STATUS_OK;
out:
    free(ie);

    return ret;
}

static VCN ntfs_icx_parent_vcn(ntfs_index_context *icx)
{
    return icx->parent_vcn[icx->pindex];
}

static VCN ntfs_icx_parent_pos(ntfs_index_context *icx)
{
    return icx->parent_pos[icx->pindex];
}


static int ntfs_ir_insert_median(ntfs_index_context *icx, INDEX_ENTRY *median,
                                 VCN new_vcn)
{
    u32 new_size;
    int ret;

    fs_log_trace01("Entering\n");

    icx->ir = ntfs_ir_lookup2(icx->ni, icx->name, icx->name_len);
    if (!icx->ir)
    {
        return STATUS_ERROR;
    }

    new_size = le32_to_cpu(icx->ir->index.index_length) +
               le16_to_cpu(median->length);
    if (!(median->ie_flags & INDEX_ENTRY_NODE))
    {
        new_size += sizeof(VCN);
    }

    ret = ntfs_ir_make_space(icx, new_size);
    if (ret != STATUS_OK)
    {
        return ret;
    }

    icx->ir = ntfs_ir_lookup2(icx->ni, icx->name, icx->name_len);
    if (!icx->ir)
    {
        return STATUS_ERROR;
    }

    return ntfs_ih_insert(&icx->ir->index, median, new_vcn,
                          ntfs_icx_parent_pos(icx));
}

static int ntfs_ib_split(ntfs_index_context *icx, INDEX_BLOCK *ib);

/**
 * On success return STATUS_OK or STATUS_KEEP_SEARCHING.
 * On error return STATUS_ERROR.
 */
static int ntfs_ib_insert(ntfs_index_context *icx, INDEX_ENTRY *ie, VCN new_vcn)
{
    INDEX_BLOCK *ib;
    u32 idx_size, allocated_size;
    int err = STATUS_ERROR;
    VCN old_vcn;

    fs_log_trace01("Entering\n");

    ib = ntfs_malloc(icx->block_size);
    if (!ib)
    {
        return -1;
    }

    old_vcn = ntfs_icx_parent_vcn(icx);

    if (ntfs_ib_read(icx, old_vcn, ib))
    {
        goto err_out;
    }

    idx_size       = le32_to_cpu(ib->index.index_length);
    allocated_size = le32_to_cpu(ib->index.allocated_size);
    /* FIXME: sizeof(VCN) should be included only if ie has no VCN */
    if (idx_size + le16_to_cpu(ie->length) + sizeof(VCN) > allocated_size)
    {
        err = ntfs_ib_split(icx, ib);
        if (err == STATUS_OK)
        {
            err = STATUS_KEEP_SEARCHING;
        }
        goto err_out;
    }

    if (ntfs_ih_insert(&ib->index, ie, new_vcn, ntfs_icx_parent_pos(icx)))
    {
        goto err_out;
    }

    if (ntfs_ib_write(icx, ib))
    {
        goto err_out;
    }

    err = STATUS_OK;
err_out:
    free(ib);
    return err;
}

/**
 * ntfs_ib_split - Split an index block
 *
 * On success return STATUS_OK or STATUS_KEEP_SEARCHING.
 * On error return is STATUS_ERROR.
 */
static int ntfs_ib_split(ntfs_index_context *icx, INDEX_BLOCK *ib)
{
    INDEX_ENTRY *median;
    VCN new_vcn;
    int ret;

    fs_log_trace01("Entering\n");

    if (ntfs_icx_parent_dec(icx))
    {
        return STATUS_ERROR;
    }

    median  = ntfs_ie_get_median(&ib->index);
    new_vcn = ntfs_ibm_get_free(icx);
    if (new_vcn == -1)
    {
        return STATUS_ERROR;
    }

    if (ntfs_ib_copy_tail(icx, ib, median, new_vcn))
    {
        ntfs_ibm_clear(icx, new_vcn);
        return STATUS_ERROR;
    }

    if (ntfs_icx_parent_vcn(icx) == VCN_INDEX_ROOT_PARENT)
    {
        ret = ntfs_ir_insert_median(icx, median, new_vcn);
    }
    else
    {
        ret = ntfs_ib_insert(icx, median, new_vcn);
    }

    if (ret != STATUS_OK)
    {
        ntfs_ibm_clear(icx, new_vcn);
        return ret;
    }

    ret = ntfs_ib_cut_tail(icx, ib, median);

    return ret;
}


static int ntfs_ie_add(ntfs_index_context *icx, INDEX_ENTRY *ie)
{
    INDEX_HEADER *ih;
    int allocated_size, new_size;
    int ret = STATUS_ERROR;

#ifdef DEBUG
    char *fn;
    fn = ntfs_ie_filename_get(ie);
    fs_log_trace01("file: '%s'\n", fn);
    ntfs_attr_name_free(&fn);
#endif

    while (1)
    {

        if (!ntfs_index_lookup(&ie->key, le16_to_cpu(ie->key_length), icx))
        {
            fs_err = EEXIST;
            fs_log_perror("Index already have such entry");
            goto err_out;
        }
        if (fs_err != ENOENT)
        {
            fs_log_perror("Failed to find place for new entry");
            goto err_out;
        }

        if (icx->is_in_root)
        {
            ih = &icx->ir->index;
        }
        else
        {
            ih = &icx->ib->index;
        }

        allocated_size = le32_to_cpu(ih->allocated_size);
        new_size = le32_to_cpu(ih->index_length) + le16_to_cpu(ie->length);

        if (new_size <= allocated_size)
        {
            break;
        }

        fs_log_trace01("index block sizes: allocated: %d  needed: %d\n",
                       allocated_size, new_size);

        if (icx->is_in_root)
        {
            if (ntfs_ir_make_space(icx, new_size) == STATUS_ERROR)
            {
                goto err_out;
            }
        }
        else
        {
            if (ntfs_ib_split(icx, icx->ib) == STATUS_ERROR)
            {
                goto err_out;
            }
        }

        ntfs_inode_mark_dirty(icx->actx->ntfs_ino);
        ntfs_index_ctx_reinit(icx);
    }

    ntfs_ie_insert(ih, ie, icx->entry);
    ntfs_index_entry_mark_dirty(icx);

    ret = STATUS_OK;
err_out:
    fs_log_trace01("%s\n", ret ? "Failed" : "Done");
    return ret;
}

static int ntfs_ih_takeout(ntfs_index_context *icx, INDEX_HEADER *ih,
                           INDEX_ENTRY *ie, INDEX_BLOCK *ib)
{
    INDEX_ENTRY *ie_roam;
    int ret = STATUS_ERROR;

    fs_log_trace01("Entering\n");

    ie_roam = ntfs_ie_dup_novcn(ie);
    if (!ie_roam)
    {
        return STATUS_ERROR;
    }

    ntfs_ie_delete(ih, ie);

    if (ntfs_icx_parent_vcn(icx) == VCN_INDEX_ROOT_PARENT)
    {
        ntfs_inode_mark_dirty(icx->actx->ntfs_ino);
    }
    else if (ntfs_ib_write(icx, ib))
    {
        goto out;
    }

    ntfs_index_ctx_reinit(icx);

    ret = ntfs_ie_add(icx, ie_roam);
out:
    free(ie_roam);
    return ret;
}

/**
 *  Used if an empty index block to be deleted has END entry as the parent
 *  in the INDEX_ROOT which is the only one there.
 */
static void ntfs_ir_leafify(ntfs_index_context *icx, INDEX_HEADER *ih)
{
    INDEX_ENTRY *ie;

    fs_log_trace01("Entering\n");

    ie = ntfs_ie_get_first(ih);
    ie->ie_flags &= ~INDEX_ENTRY_NODE;
    ie->length = cpu_to_le16(le16_to_cpu(ie->length) - sizeof(VCN));

    ih->index_length = cpu_to_le32(le32_to_cpu(ih->index_length) - sizeof(VCN));
    ih->ih_flags &= ~LARGE_INDEX;

    /* Not fatal error */
    ntfs_ir_truncate(icx, le32_to_cpu(ih->index_length));
}

/**
 *  Used if an empty index block to be deleted has END entry as the parent
 *  in the INDEX_ROOT which is not the only one there.
 */
static int ntfs_ih_reparent_end(ntfs_index_context *icx, INDEX_HEADER *ih,
                                INDEX_BLOCK *ib)
{
    INDEX_ENTRY *ie, *ie_prev;

    fs_log_trace01("Entering\n");

    ie = ntfs_ie_get_by_pos(ih, ntfs_icx_parent_pos(icx));
    ie_prev = ntfs_ie_prev(ih, ie);

    ntfs_ie_set_vcn(ie, ntfs_ie_get_vcn(ie_prev));

    return ntfs_ih_takeout(icx, ih, ie_prev, ib);
}

static int ntfs_index_rm_leaf(ntfs_index_context *icx)
{
    INDEX_BLOCK *ib = NULL;
    INDEX_HEADER *parent_ih;
    INDEX_ENTRY *ie;
    int ret = STATUS_ERROR;

    fs_log_trace01("pindex: %d\n", icx->pindex);

    if (ntfs_icx_parent_dec(icx))
    {
        return STATUS_ERROR;
    }

    if (ntfs_ibm_clear(icx, icx->parent_vcn[icx->pindex + 1]))
    {
        return STATUS_ERROR;
    }

    if (ntfs_icx_parent_vcn(icx) == VCN_INDEX_ROOT_PARENT)
    {
        parent_ih = &icx->ir->index;
    }
    else
    {
        ib = ntfs_malloc(icx->block_size);
        if (!ib)
        {
            return STATUS_ERROR;
        }

        if (ntfs_ib_read(icx, ntfs_icx_parent_vcn(icx), ib))
        {
            goto out;
        }

        parent_ih = &ib->index;
    }

    ie = ntfs_ie_get_by_pos(parent_ih, ntfs_icx_parent_pos(icx));
    if (!ntfs_ie_end(ie))
    {
        ret = ntfs_ih_takeout(icx, parent_ih, ie, ib);
        goto out;
    }

    if (ntfs_ih_zero_entry(parent_ih))
    {

        if (ntfs_icx_parent_vcn(icx) == VCN_INDEX_ROOT_PARENT)
        {
            ntfs_ir_leafify(icx, parent_ih);
            goto ok;
        }

        ret = ntfs_index_rm_leaf(icx);
        goto out;
    }

    if (ntfs_ih_reparent_end(icx, parent_ih, ib))
    {
        goto out;
    }
ok:
    ret = STATUS_OK;
out:
    free(ib);
    return ret;
}

static int ntfs_index_rm_node(ntfs_index_context *icx)
{
    int entry_pos, pindex;
    VCN vcn;
    INDEX_BLOCK *ib = NULL;
    INDEX_ENTRY *ie_succ, *ie, *entry = icx->entry;
    INDEX_HEADER *ih;
    u32 new_size;
    int delta, ret = STATUS_ERROR;

    fs_log_trace01("Entering\n");

    if (!icx->ia_na)
    {
        icx->ia_na = ntfs_ia_open(icx, icx->ni);
        if (!icx->ia_na)
        {
            return STATUS_ERROR;
        }
    }

    ib = ntfs_malloc(icx->block_size);
    if (!ib)
    {
        return STATUS_ERROR;
    }

    ie_succ = ntfs_ie_get_next(icx->entry);
    entry_pos = icx->parent_pos[icx->pindex]++;
    pindex = icx->pindex;
descend:
    vcn = ntfs_ie_get_vcn(ie_succ);
    if (ntfs_ib_read(icx, vcn, ib))
    {
        goto out;
    }

    ie_succ = ntfs_ie_get_first(&ib->index);

    if (ntfs_icx_parent_inc(icx))
    {
        goto out;
    }

    icx->parent_vcn[icx->pindex] = vcn;
    icx->parent_pos[icx->pindex] = 0;

    if ((ib->index.ih_flags & NODE_MASK) == INDEX_NODE)
    {
        goto descend;
    }

    if (ntfs_ih_zero_entry(&ib->index))
    {
        fs_err = EIO;
        fs_log_perror("Empty index block");
        goto out;
    }

    ie = ntfs_ie_dup(ie_succ);
    if (!ie)
    {
        goto out;
    }

    if (ntfs_ie_add_vcn(&ie))
    {
        goto out2;
    }

    ntfs_ie_set_vcn(ie, ntfs_ie_get_vcn(icx->entry));

    if (icx->is_in_root)
    {
        ih = &icx->ir->index;
    }
    else
    {
        ih = &icx->ib->index;
    }

    delta = le16_to_cpu(ie->length) - le16_to_cpu(icx->entry->length);
    new_size = le32_to_cpu(ih->index_length) + delta;
    if (delta > 0)
    {
        if (icx->is_in_root)
        {
            ret = ntfs_ir_make_space(icx, new_size);
            if (ret != STATUS_OK)
            {
                goto out2;
            }

            ih = &icx->ir->index;
            entry = ntfs_ie_get_by_pos(ih, entry_pos);

        }
        else if (new_size > le32_to_cpu(ih->allocated_size))
        {
            icx->pindex = pindex;
            ret = ntfs_ib_split(icx, icx->ib);
            if (ret == STATUS_OK)
            {
                ret = STATUS_KEEP_SEARCHING;
            }
            goto out2;
        }
    }

    ntfs_ie_delete(ih, entry);
    ntfs_ie_insert(ih, ie, entry);

    if (icx->is_in_root)
    {
        if (ntfs_ir_truncate(icx, new_size))
        {
            goto out2;
        }
    }
    else if (ntfs_icx_ib_write(icx))
    {
        goto out2;
    }

    ntfs_ie_delete(&ib->index, ie_succ);

    if (ntfs_ih_zero_entry(&ib->index))
    {
        if (ntfs_index_rm_leaf(icx))
        {
            goto out2;
        }
    }
    else if (ntfs_ib_write(icx, ib))
    {
        goto out2;
    }

    ret = STATUS_OK;
out2:
    free(ie);
out:
    free(ib);
    return ret;
}

/**
 * ntfs_index_rm - remove entry from the index
 * @icx:    index context describing entry to delete
 *
 * Delete entry described by @icx from the index. Index context is always
 * reinitialized after use of this function, so it can be used for index
 * lookup once again.
 *
 * Return 0 on success or -1 on error with fs_err set to the error code.
 */
static int ntfs_index_rm(ntfs_index_context *icx)
{
    INDEX_HEADER *ih;
    int err, ret = STATUS_OK;

    fs_log_trace01("Entering\n");

    if (!icx || (!icx->ib && !icx->ir) || ntfs_ie_end(icx->entry))
    {
        fs_log_error("Invalid arguments.\n");
        fs_err = EINVAL;
        goto err_out;
    }
    if (icx->is_in_root)
    {
        ih = &icx->ir->index;
    }
    else
    {
        ih = &icx->ib->index;
    }

    if (icx->entry->ie_flags & INDEX_ENTRY_NODE)
    {

        ret = ntfs_index_rm_node(icx);

    }
    else if (icx->is_in_root || !ntfs_ih_one_entry(ih))
    {

        ntfs_ie_delete(ih, icx->entry);

        if (icx->is_in_root)
        {
            err = ntfs_ir_truncate(icx, le32_to_cpu(ih->index_length));
            if (err != STATUS_OK)
            {
                goto err_out;
            }
        }
        else if (ntfs_icx_ib_write(icx))
        {
            goto err_out;
        }
    }
    else
    {
        if (ntfs_index_rm_leaf(icx))
        {
            goto err_out;
        }
    }
out:
    return ret;
err_out:
    ret = STATUS_ERROR;
    goto out;
}

/**
 * ntfs_index_ctx_reinit - reinitialize an index context
 * @icx:    index context to reinitialize
 *
 * Reinitialize the index context @icx so it can be used for ntfs_index_lookup.
 */
void ntfs_index_ctx_reinit(ntfs_index_context *icx)
{
    fs_log_trace01("Entering\n");

    ntfs_index_ctx_free(icx);

    *icx = (ntfs_index_context)
    {
        .ni = icx->ni,
        .name = icx->name,
        .name_len = icx->name_len,
    };
}

char *ntfs_ie_filename_get(INDEX_ENTRY *ie)
{
    FILE_NAME_ATTR *fn;

    fn = (FILE_NAME_ATTR *)&ie->key;
    return ntfs_attr_name_get(fn->file_name, fn->file_name_length);
}

void ntfs_ie_filename_dump(INDEX_ENTRY *ie)
{
    char *s;

    s = ntfs_ie_filename_get(ie);
    fs_log_debug("'%s' ", s);
    ntfs_attr_name_free(&s);
}

void ntfs_ih_filename_dump(INDEX_HEADER *ih)
{
    INDEX_ENTRY *ie;

    fs_log_trace01("Entering\n");

    ie = ntfs_ie_get_first(ih);
    while (!ntfs_ie_end(ie))
    {
        ntfs_ie_filename_dump(ie);
        ie = ntfs_ie_get_next(ie);
    }
}

/**
 * ntfs_index_add_filename - add filename to directory index
 * @ni:     ntfs inode describing directory to which index add filename
 * @fn:     FILE_NAME attribute to add
 * @mref:   reference of the inode which @fn describes
 *
 * Return 0 on success or -1 on error with fs_err set to the error code.
 */
int ntfs_index_add_filename(ntfs_inode *ni, FILE_NAME_ATTR *fn, MFT_REF mref)
{
    INDEX_ENTRY *ie;
    ntfs_index_context *icx;
    int fn_size, ie_size, err, ret = -1;

    fs_log_trace01("Entering\n");

    if (!ni || !fn)
    {
        fs_log_error("Invalid arguments.\n");
        fs_err = EINVAL;
        return -1;
    }

    fn_size = (fn->file_name_length * sizeof(ntfschar)) +
              sizeof(FILE_NAME_ATTR);
    ie_size = (sizeof(INDEX_ENTRY_HEADER) + fn_size + 7) & ~7;

    ie = ntfs_calloc(ie_size);
    if (!ie)
    {
        return -1;
    }

    ie->indexed_file = cpu_to_le64(mref);
    ie->length   = cpu_to_le16(ie_size);
    ie->key_length   = cpu_to_le16(fn_size);
    memcpy(&ie->key, fn, fn_size);

    icx = ntfs_index_ctx_get(ni, NTFS_INDEX_I30, 4);
    if (!icx)
    {
        goto out;
    }

    ret = ntfs_ie_add(icx, ie);
    err = fs_err;
    ntfs_index_ctx_put(icx);
    fs_err = err;
out:
    free(ie);
    return ret;
}

int ntfs_index_remove(ntfs_inode *ni, const void *key, const int keylen)
{
    int ret = STATUS_ERROR;
    ntfs_index_context *icx;

    icx = ntfs_index_ctx_get(ni, NTFS_INDEX_I30, 4);
    if (!icx)
    {
        return -1;
    }

    while (1)
    {

        if (ntfs_index_lookup(key, keylen, icx))
        {
            goto err_out;
        }

        if (((FILE_NAME_ATTR *)icx->data)->file_attributes &
            FILE_ATTR_REPARSE_POINT)
        {
            fs_err = EOPNOTSUPP;
            goto err_out;
        }

        ret = ntfs_index_rm(icx);
        if (ret == STATUS_ERROR)
        {
            goto err_out;
        }
        else if (ret == STATUS_OK)
        {
            break;
        }

        ntfs_inode_mark_dirty(icx->actx->ntfs_ino);
        ntfs_index_ctx_reinit(icx);
    }

    ntfs_inode_mark_dirty(icx->actx->ntfs_ino);
out:
    ntfs_index_ctx_put(icx);
    return ret;
err_out:
    ret = STATUS_ERROR;
    fs_log_perror("Delete failed");
    goto out;
}

/**
 * ntfs_index_root_get - read the index root of an attribute
 * @ni:     open ntfs inode in which the ntfs attribute resides
 * @attr:   attribute for which we want its index root
 *
 * This function will read the related index root an ntfs attribute.
 *
 * On success a buffer is allocated with the content of the index root
 * and which needs to be freed when it's not needed anymore.
 *
 * On error NULL is returned with fs_err set to the error code.
 */
INDEX_ROOT *ntfs_index_root_get(ntfs_inode *ni, ATTR_RECORD *attr)
{
    ntfs_attr_search_ctx *ctx;
    ntfschar *name;
    INDEX_ROOT *root = NULL;

    name = (ntfschar *)((u8 *)attr + le16_to_cpu(attr->name_offset));

    if (!ntfs_ir_lookup(ni, name, attr->name_length, &ctx))
    {
        return NULL;
    }

    root = ntfs_malloc(sizeof(INDEX_ROOT));
    if (!root)
    {
        goto out;
    }

    *root = *((INDEX_ROOT *)((u8 *)ctx->attr +
                             le16_to_cpu(ctx->attr->value_offset)));
out:
    ntfs_attr_put_search_ctx(ctx);
    return root;
}

#endif  /* FSYS_NTFS_RW */
