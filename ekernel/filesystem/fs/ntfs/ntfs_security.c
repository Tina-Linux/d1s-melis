/**
 * security.c - Handling security/ACLs in NTFS.  Originated from the Linux-NTFS project.
 *
 * Copyright (c) 2004 Anton Altaparmakov
 * Copyright (c) 2005-2006 Szabolcs Szakacsits
 * Copyright (c) 2006 Yura Pakhuchiy
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
#include "types.h"
#include "layout.h"
#include "attrib.h"
#include "security.h"
#include "ntfs.h"
#include "bitmap.h"
#include "errno.h"

#include "fsys_libs.h"
#include "fsys_debug.h"

#if defined CONFIG_FSYS_NTFS_RW
/*
 * The zero GUID.
 */
static const GUID __zero_guid = { const_cpu_to_le32(0), const_cpu_to_le16(0),
                                  const_cpu_to_le16(0), { 0, 0, 0, 0, 0, 0, 0, 0 }
                                };
const GUID *const zero_guid = &__zero_guid;

/**
 * ntfs_guid_is_zero - check if a GUID is zero
 * @guid:   [IN] guid to check
 *
 * Return TRUE if @guid is a valid pointer to a GUID and it is the zero GUID
 * and FALSE otherwise.
 */
BOOL ntfs_guid_is_zero(const GUID *guid)
{
    return (memcmp(guid, zero_guid, sizeof(*zero_guid)));
}

/**
 * ntfs_guid_to_mbs - convert a GUID to a multi byte string
 * @guid:   [IN]  guid to convert
 * @guid_str:   [OUT] string in which to return the GUID (optional)
 *
 * Convert the GUID pointed to by @guid to a multi byte string of the form
 * "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX".  Therefore, @guid_str (if not NULL)
 * needs to be able to store at least 37 bytes.
 *
 * If @guid_str is not NULL it will contain the converted GUID on return.  If
 * it is NULL a string will be allocated and this will be returned.  The caller
 * is responsible for free()ing the string in that case.
 *
 * On success return the converted string and on failure return NULL with fs_err
 * set to the error code.
 */
char *ntfs_guid_to_mbs(const GUID *guid, char *guid_str)
{
    char *_guid_str;
    int res;

    if (!guid)
    {
        fs_err = EINVAL;
        return NULL;
    }
    _guid_str = guid_str;
    if (!_guid_str)
    {
        _guid_str = ntfs_malloc(37);
        if (!_guid_str)
        {
            return _guid_str;
        }
    }
    res = snprintf(_guid_str, 37,
                   "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                   (unsigned int)le32_to_cpu(guid->data1),
                   le16_to_cpu(guid->data2), le16_to_cpu(guid->data3),
                   guid->data4[0], guid->data4[1],
                   guid->data4[2], guid->data4[3], guid->data4[4],
                   guid->data4[5], guid->data4[6], guid->data4[7]);
    if (res == 36)
    {
        return _guid_str;
    }
    if (!guid_str)
    {
        free(_guid_str);
    }
    fs_err = EINVAL;
    return NULL;
}

/**
 * ntfs_sid_to_mbs_size - determine maximum size for the string of a SID
 * @sid:    [IN]  SID for which to determine the maximum string size
 *
 * Determine the maximum multi byte string size in bytes which is needed to
 * store the standard textual representation of the SID pointed to by @sid.
 * See ntfs_sid_to_mbs(), below.
 *
 * On success return the maximum number of bytes needed to store the multi byte
 * string and on failure return -1 with fs_err set to the error code.
 */
int ntfs_sid_to_mbs_size(const SID *sid)
{
    int size, i;

    if (!ntfs_sid_is_valid(sid))
    {
        fs_err = EINVAL;
        return -1;
    }
    /* Start with "S-". */
    size = 2;
    /*
     * Add the SID_REVISION.  Hopefully the compiler will optimize this
     * away as SID_REVISION is a constant.
     */
    for (i = SID_REVISION; i > 0; i /= 10)
    {
        size++;
    }
    /* Add the "-". */
    size++;
    /*
     * Add the identifier authority.  If it needs to be in decimal, the
     * maximum is 2^32-1 = 4294967295 = 10 characters.  If it needs to be
     * in hexadecimal, then maximum is 0x665544332211 = 14 characters.
     */
    if (!sid->identifier_authority.high_part)
    {
        size += 10;
    }
    else
    {
        size += 14;
    }
    /*
     * Finally, add the sub authorities.  For each we have a "-" followed
     * by a decimal which can be up to 2^32-1 = 4294967295 = 10 characters.
     */
    size += (1 + 10) * sid->sub_authority_count;
    /* We need the zero byte at the end, too. */
    size++;
    return size * sizeof(char);
}

/**
 * ntfs_sid_to_mbs - convert a SID to a multi byte string
 * @sid:        [IN]  SID to convert
 * @sid_str:        [OUT] string in which to return the SID (optional)
 * @sid_str_size:   [IN]  size in bytes of @sid_str
 *
 * Convert the SID pointed to by @sid to its standard textual representation.
 * @sid_str (if not NULL) needs to be able to store at least
 * ntfs_sid_to_mbs_size() bytes.  @sid_str_size is the size in bytes of
 * @sid_str if @sid_str is not NULL.
 *
 * The standard textual representation of the SID is of the form:
 *  S-R-I-S-S...
 * Where:
 *    - The first "S" is the literal character 'S' identifying the following
 *  digits as a SID.
 *    - R is the revision level of the SID expressed as a sequence of digits
 *  in decimal.
 *    - I is the 48-bit identifier_authority, expressed as digits in decimal,
 *  if I < 2^32, or hexadecimal prefixed by "0x", if I >= 2^32.
 *    - S... is one or more sub_authority values, expressed as digits in
 *  decimal.
 *
 * If @sid_str is not NULL it will contain the converted SUID on return.  If it
 * is NULL a string will be allocated and this will be returned.  The caller is
 * responsible for free()ing the string in that case.
 *
 * On success return the converted string and on failure return NULL with fs_err
 * set to the error code.
 */
char *ntfs_sid_to_mbs(const SID *sid, char *sid_str, size_t sid_str_size)
{
    u64 u;
    char *s;
    int i, j, cnt;

    /*
     * No need to check @sid if !@sid_str since ntfs_sid_to_mbs_size() will
     * check @sid, too.  8 is the minimum SID string size.
     */
    if (sid_str && (sid_str_size < 8 || !ntfs_sid_is_valid(sid)))
    {
        fs_err = EINVAL;
        return NULL;
    }
    /* Allocate string if not provided. */
    if (!sid_str)
    {
        cnt = ntfs_sid_to_mbs_size(sid);
        if (cnt < 0)
        {
            return NULL;
        }
        s = ntfs_malloc(cnt);
        if (!s)
        {
            return s;
        }
        sid_str = s;
        /* So we know we allocated it. */
        sid_str_size = 0;
    }
    else
    {
        s = sid_str;
        cnt = sid_str_size;
    }
    /* Start with "S-R-". */
    i = snprintf(s, cnt, "S-%hhu-", (unsigned char)sid->revision);
    if (i < 0 || i >= cnt)
    {
        goto err_out;
    }
    s += i;
    cnt -= i;
    /* Add the identifier authority. */
    for (u = i = 0, j = 40; i < 6; i++, j -= 8)
    {
        u += (u64)sid->identifier_authority.value[i] << j;
    }
    if (!sid->identifier_authority.high_part)
    {
        i = snprintf(s, cnt, "%lu", (unsigned long)u);
    }
    else
    {
        i = snprintf(s, cnt, "0x%llx", (unsigned long long)u);
    }
    if (i < 0 || i >= cnt)
    {
        goto err_out;
    }
    s += i;
    cnt -= i;
    /* Finally, add the sub authorities. */
    for (j = 0; j < sid->sub_authority_count; j++)
    {
        i = snprintf(s, cnt, "-%u", (unsigned int)
                     le32_to_cpu(sid->sub_authority[j]));
        if (i < 0 || i >= cnt)
        {
            goto err_out;
        }
        s += i;
        cnt -= i;
    }
    return sid_str;
err_out:
    if (i >= cnt)
    {
        i = EMSGSIZE;
    }
    else
    {
        i = fs_err;
    }
    if (!sid_str_size)
    {
        free(sid_str);
    }
    fs_err = i;
    return NULL;
}

/**
 * ntfs_generate_guid - generatates a random current guid.
 * @guid:   [OUT]   pointer to a GUID struct to hold the generated guid.
 *
 * perhaps not a very good random number generator though...
 */
void ntfs_generate_guid(GUID *guid)
{
    unsigned int i;
    u8 *p = (u8 *)guid;

    for (i = 0; i < sizeof(GUID); i++)
    {
        p[i] = (u8)(random() & 0xFF);
        if (i == 7)
        {
            p[7] = (p[7] & 0x0F) | 0x40;
        }
        if (i == 8)
        {
            p[8] = (p[8] & 0x3F) | 0x80;
        }
    }
}

int ntfs_sd_add_everyone(ntfs_inode *ni)
{
    SECURITY_DESCRIPTOR_ATTR *sd;
    ACL *acl;
    ACCESS_ALLOWED_ACE *ace;
    SID *sid;
    int ret, sd_len;

    /* Create SECURITY_DESCRIPTOR attribute (everyone has full access). */
    /*
     * Calculate security descriptor length. We have 2 sub-authorities in
     * owner and group SIDs, but structure SID contain only one, so add
     * 4 bytes to every SID.
     */
    sd_len = sizeof(SECURITY_DESCRIPTOR_ATTR) + 2 * (sizeof(SID) + 4) +
             sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE);
    sd = ntfs_calloc(sd_len);
    if (!sd)
    {
        return -1;
    }

    sd->revision = 1;
    sd->control = SE_DACL_PRESENT | SE_SELF_RELATIVE;

    sid = (SID *)((u8 *)sd + sizeof(SECURITY_DESCRIPTOR_ATTR));
    sid->revision = 1;
    sid->sub_authority_count = 2;
    sid->sub_authority[0] = cpu_to_le32(SECURITY_BUILTIN_DOMAIN_RID);
    sid->sub_authority[1] = cpu_to_le32(DOMAIN_ALIAS_RID_ADMINS);
    sid->identifier_authority.value[5] = 5;
    sd->owner = cpu_to_le32((u8 *)sid - (u8 *)sd);

    sid = (SID *)((u8 *)sid + sizeof(SID) + 4);
    sid->revision = 1;
    sid->sub_authority_count = 2;
    sid->sub_authority[0] = cpu_to_le32(SECURITY_BUILTIN_DOMAIN_RID);
    sid->sub_authority[1] = cpu_to_le32(DOMAIN_ALIAS_RID_ADMINS);
    sid->identifier_authority.value[5] = 5;
    sd->group = cpu_to_le32((u8 *)sid - (u8 *)sd);

    acl = (ACL *)((u8 *)sid + sizeof(SID) + 4);
    acl->revision = 2;
    acl->size = cpu_to_le16(sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE));
    acl->ace_count = cpu_to_le16(1);
    sd->dacl = cpu_to_le32((u8 *)acl - (u8 *)sd);

    ace = (ACCESS_ALLOWED_ACE *)((u8 *)acl + sizeof(ACL));
    ace->type = ACCESS_ALLOWED_ACE_TYPE;
    ace->flags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
    ace->size = cpu_to_le16(sizeof(ACCESS_ALLOWED_ACE));
    ace->mask = cpu_to_le32(0x1f01ff); /* FIXME */
    ace->sid.revision = 1;
    ace->sid.sub_authority_count = 1;
    ace->sid.sub_authority[0] = 0;
    ace->sid.identifier_authority.value[5] = 1;

    ret = ntfs_attr_add(ni, AT_SECURITY_DESCRIPTOR, AT_UNNAMED, 0, (u8 *)sd,
                        sd_len);
    if (ret)
    {
        fs_log_perror("Failed to add SECURITY_DESCRIPTOR\n");
    }

    free(sd);
    return ret;
}

/**
 * ntfs_security_hash - calculate the hash of a security descriptor
 * @sd:         self-relative security descriptor whose hash to calculate
 * @length:     size in bytes of the security descritor @sd
 *
 * Calculate the hash of the self-relative security descriptor @sd of length
 * @length bytes.
 *
 * This hash is used in the $Secure system file as the primary key for the $SDH
 * index and is also stored in the header of each security descriptor in the
 * $SDS data stream as well as in the index data of both the $SII and $SDH
 * indexes.  In all three cases it forms part of the SDS_ENTRY_HEADER
 * structure.
 *
 * Return the calculated security hash in little endian.
 */
le32 ntfs_security_hash(const SECURITY_DESCRIPTOR_RELATIVE *sd, const u32 len)
{
    const le32 *pos = (const le32 *)sd;
    const le32 *end = pos + (len >> 2);
    u32 hash = 0;

    while (pos < end)
    {
        hash = le32_to_cpup(pos) + ntfs_rol32(hash, 3);
        pos++;
    }
    return cpu_to_le32(hash);
}
#endif  /* FSYS_NTFS_RW */
