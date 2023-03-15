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
#include "dospart.h"
#include "smbr.h"
#include "fsys_libs.h"
#include "supports.h"
#include <log.h>

static inline void put_partition(struct parsed_partitions *p, int n, unsigned int from, unsigned int size)
{
    if (n < p->limit)
    {
        p->parts[n].from = from;
        p->parts[n].size = size;
        __log(" %s%d", p->name, n);
    }
}

/*
 * Many architectures don't like unaligned accesses, while
 * the nr_sects and start_sect partition table entries are
 * at a 2 (mod 4) address.
 */
#define SYS_IND(p)          (*(&p->sys_ind))
#define NR_SECTS(p)         (*(&p->nr_sects))
#define START_SECT(p)       (*(&p->start_sect))

static inline int is_extended_partition(struct partition *p)
{
    return (SYS_IND(p) == DOS_EXTENDED_PARTITION   ||
            SYS_IND(p) == WIN98_EXTENDED_PARTITION ||
            SYS_IND(p) == LINUX_EXTENDED_PARTITION);
}

#define MSDOS_LABEL_MAGIC1  0x55
#define MSDOS_LABEL_MAGIC2  0xAA

static inline int msdos_magic_present(unsigned char *p)
{
    return (p[0] == MSDOS_LABEL_MAGIC1 && p[1] == MSDOS_LABEL_MAGIC2);
}

/*
 * get block device hardware sector size.
 */
int blkdev_hardsect_size(__hdle hDev)
{
    int  sector_size;
    __dev_blkinfo_t blkinf;

    /* First of all, get hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and bigger then 512.
     */
    memset(&blkinf, 0, sizeof(__dev_blkinfo_t));
    esDEV_Ioctl(hDev, DEV_CMD_GET_INFO, 0, &blkinf);
    sector_size = blkinf.secsize;
    if (sector_size < 512 || !is_power_of_2(sector_size))
    {
        /* invalid sector size, default is 512 byte */
        sector_size = 512;
    }
    return sector_size;
}

unsigned char *read_dev_sector(__hdle hDev, __u32 sec_pos, char **return_p)
{
    char  *p;
    int    ret;
    int    secsize = 0;

    /* First of all, get hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and bigger then 512.
     */
    secsize = blkdev_hardsect_size(hDev);

    /* maybe palloc will faster */
    p = malloc(secsize);
    ret = esDEV_Read(p, sec_pos, 1, hDev);
    if (ret != 1)
    {
        free(p);
        p = NULL;
    }

    *return_p = p;
    return (unsigned char *) p;
}

void put_dev_sector(__hdle hDev, char *p)
{
    int    secsize = 0;

    /* First of all, get hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and bigger then 512.
     */
    secsize = blkdev_hardsect_size(hDev);

    free(p);
}

/* Value is EBCDIC 'IBMA' */
#define AIX_LABEL_MAGIC1    0xC9
#define AIX_LABEL_MAGIC2    0xC2
#define AIX_LABEL_MAGIC3    0xD4
#define AIX_LABEL_MAGIC4    0xC1
static int aix_magic_present(unsigned char *p, __hdle bdev)
{
    struct partition *pt = (struct partition *)(p + 0x1be);
    char *sect;
    unsigned char *d;
    int slot, ret = 0;

    if (!(p[0] == AIX_LABEL_MAGIC1 &&
          p[1] == AIX_LABEL_MAGIC2 &&
          p[2] == AIX_LABEL_MAGIC3 &&
          p[3] == AIX_LABEL_MAGIC4))
    {
        return 0;
    }
    /* Assume the partition table is valid if Linux partitions exists */
    for (slot = 1; slot <= 4; slot++, pt++)
    {
        if (pt->sys_ind == LINUX_SWAP_PARTITION ||
            pt->sys_ind == LINUX_RAID_PARTITION ||
            pt->sys_ind == LINUX_DATA_PARTITION ||
            pt->sys_ind == LINUX_LVM_PARTITION  ||
            is_extended_partition(pt))
        {
            return 0;
        }
    }
    d = read_dev_sector(bdev, 7, &sect);
    if (d)
    {
        if (d[0] == '_' && d[1] == 'L' && d[2] == 'V' && d[3] == 'M')
        {
            ret = 1;
        }
        put_dev_sector(bdev, sect);
    }
    return ret;
}

/*
 * Create devices for each logical partition in an extended partition.
 * The logical partitions form a linked list, with each entry being
 * a partition table with two entries.  The first entry
 * is the real data partition (with a start relative to the partition
 * table start).  The second is a pointer to the next logical partition
 * (with a start relative to the entire extended partition).
 * We do not create a Linux partition for the partition tables, but
 * only for the actual data partitions.
 */
static void parse_extended(struct parsed_partitions *state, __hdle bdev,
                           unsigned int first_sector, unsigned int first_size)
{
    struct partition *p;
    char *sect;
    unsigned char *data;
    unsigned int this_sector, this_size;
    int loopct = 0;     /* number of links followed
                           without finding a data partition */
    int i;

    this_sector = first_sector;
    this_size = first_size;

    while (1)
    {
        if (++loopct > 100)
        {
            return;
        }
        if (state->next == state->limit)
        {
            return;
        }
        data = read_dev_sector(bdev, this_sector, &sect);
        if (!data)
        {
            return;
        }

        if (!msdos_magic_present(data + 510))
        {
            goto done;
        }

        p = (struct partition *)(data + 0x1be);

        /*
         * Usually, the first entry is the real data partition,
         * the 2nd entry is the next extended partition, or empty,
         * and the 3rd and 4th entries are unused.
         * However, DRDOS sometimes has the extended partition as
         * the first entry (when the data partition is empty),
         * and OS/2 seems to use all four entries.
         */

        /*
         * First process the data partition(s)
         */
        for (i = 0; i < 4; i++, p++)
        {
            unsigned int offs, size, next;
            if (!NR_SECTS(p) || is_extended_partition(p))
            {
                continue;
            }

            /* Check the 3rd and 4th entries -
               these sometimes contain random garbage */
            offs = START_SECT(p);
            size = NR_SECTS(p);
            next = this_sector + offs;
            if (i >= 2)
            {
                if (offs + size > this_size)
                {
                    continue;
                }
                if (next < first_sector)
                {
                    continue;
                }
                if (next + size > first_sector + first_size)
                {
                    continue;
                }
            }
            put_partition(state, state->next, next, size);
            if (SYS_IND(p) == LINUX_RAID_PARTITION)
            {
                state->parts[state->next].flags = ADDPART_FLAG_RAID;
            }
            loopct = 0;
            if (++state->next == state->limit)
            {
                goto done;
            }
        }
        /*
         * Next, process the (first) extended partition, if present.
         * (So far, there seems to be no reason to make
         *  parse_extended()  recursive and allow a tree
         *  of extended partitions.)
         * It should be a link to the next logical partition.
         */
        p -= 4;
        for (i = 0; i < 4; i++, p++)
            if (NR_SECTS(p) && is_extended_partition(p))
            {
                break;
            }
        if (i == 4)
        {
            goto done;    /* nothing left to do */
        }

        this_sector = first_sector + START_SECT(p);
        this_size = NR_SECTS(p);
        put_dev_sector(bdev, sect);
    }
done:
    put_dev_sector(bdev, sect);
}

/* james@bpgc.com: Solaris has a nasty indicator: 0x82 which also
   indicates linux swap.  Be careful before believing this is Solaris. */

static void parse_solaris_x86(struct parsed_partitions *state, __hdle bdev,
                              unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_SOLARIS_X86_PARTITION
    Sector sect;
    struct solaris_x86_vtoc *v;
    int i;

    v = (struct solaris_x86_vtoc *)read_dev_sector(bdev, offset + 1, &sect);
    if (!v)
    {
        return;
    }
    if (le32_to_cpu(v->v_sanity) != SOLARIS_X86_VTOC_SANE)
    {
        put_dev_sector(bdev, sect);
        return;
    }
    __log(" %s%d: <solaris:", state->name, origin);
    if (le32_to_cpu(v->v_version) != 1)
    {
        __log("  cannot handle version %d vtoc>",
              le32_to_cpu(v->v_version));
        put_dev_sector(bdev, sect);
        return;
    }
    for (i = 0; i < SOLARIS_X86_NUMSLICE && state->next < state->limit; i++)
    {
        struct solaris_x86_slice *s = &v->v_slice[i];
        if (s->s_size == 0)
        {
            continue;
        }
        __log(" [s%d]", i);
        /* solaris partitions are relative to current MS-DOS
         * one; must add the offset of the current partition */
        put_partition(state, state->next++,
                      le32_to_cpu(s->s_start) + offset,
                      le32_to_cpu(s->s_size));
    }
    put_dev_sector(bdev, sect);
    __log(" >");
#endif
}

#if defined(CONFIG_BSD_DISKLABEL)
/*
 * Create devices for BSD partitions listed in a disklabel, under a
 * dos-like partition. See parse_extended() for more information.
 */
static void parse_bsd(struct parsed_partitions *state, __hdle bdev,
                      unsigned int offset, unsigned int size, int origin,
                      char *flavour, int max_partitions)
{
    Sector sect;
    struct bsd_disklabel *l;
    struct bsd_partition *p;

    l = (struct bsd_disklabel *)read_dev_sector(bdev, offset + 1, &sect);
    if (!l)
    {
        return;
    }
    if (le32_to_cpu(l->d_magic) != BSD_DISKMAGIC)
    {
        put_dev_sector(bdev, sect);
        return;
    }
    __log(" %s%d: <%s:", state->name, origin, flavour);

    if (le16_to_cpu(l->d_npartitions) < max_partitions)
    {
        max_partitions = le16_to_cpu(l->d_npartitions);
    }
    for (p = l->d_partitions; p - l->d_partitions < max_partitions; p++)
    {
        unsigned int bsd_start, bsd_size;

        if (state->next == state->limit)
        {
            break;
        }
        if (p->p_fstype == BSD_FS_UNUSED)
        {
            continue;
        }
        bsd_start = le32_to_cpu(p->p_offset);
        bsd_size = le32_to_cpu(p->p_size);
        if (offset == bsd_start && size == bsd_size)
            /* full parent partition, we have it already */
        {
            continue;
        }
        if (offset > bsd_start || offset + size < bsd_start + bsd_size)
        {
            __log("bad subpartition - ignored");
            continue;
        }
        put_partition(state, state->next++, bsd_start, bsd_size);
    }
    put_dev_sector(bdev, sect);
    if (le16_to_cpu(l->d_npartitions) > max_partitions)
        __log(" (ignored %d more)",
              le16_to_cpu(l->d_npartitions) - max_partitions);
    __log(" >");
}
#endif

static void parse_freebsd(struct parsed_partitions *state, __hdle bdev,
                          unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_BSD_DISKLABEL
    parse_bsd(state, bdev, offset, size, origin,
              "bsd", BSD_MAXPARTITIONS);
#endif
}

static void parse_netbsd(struct parsed_partitions *state, __hdle bdev,
                         unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_BSD_DISKLABEL
    parse_bsd(state, bdev, offset, size, origin,
              "netbsd", BSD_MAXPARTITIONS);
#endif
}

static void parse_openbsd(struct parsed_partitions *state, __hdle bdev,
                          unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_BSD_DISKLABEL
    parse_bsd(state, bdev, offset, size, origin,
              "openbsd", OPENBSD_MAXPARTITIONS);
#endif
}

/*
 * Create devices for Unixware partitions listed in a disklabel, under a
 * dos-like partition. See parse_extended() for more information.
 */
static void parse_unixware(struct parsed_partitions *state, __hdle bdev,
                           unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_UNIXWARE_DISKLABEL
    Sector sect;
    struct unixware_disklabel *l;
    struct unixware_slice *p;

    l = (struct unixware_disklabel *)read_dev_sector(bdev, offset + 29, &sect);
    if (!l)
    {
        return;
    }
    if (le32_to_cpu(l->d_magic) != UNIXWARE_DISKMAGIC ||
        le32_to_cpu(l->vtoc.v_magic) != UNIXWARE_DISKMAGIC2)
    {
        put_dev_sector(bdev, sect);
        return;
    }
    __log(" %s%d: <unixware:", state->name, origin);
    p = &l->vtoc.v_slice[1];
    /* I omit the 0th slice as it is the same as whole disk. */
    while (p - &l->vtoc.v_slice[0] < UNIXWARE_NUMSLICE)
    {
        if (state->next == state->limit)
        {
            break;
        }

        if (p->s_label != UNIXWARE_FS_UNUSED)
            put_partition(state, state->next++,
                          START_SECT(p), NR_SECTS(p));
        p++;
    }
    put_dev_sector(bdev, sect);
    __log(" >");
#endif
}

/*
 * Minix 2.0.0/2.0.2 subpartition support.
 * Anand Krishnamurthy <anandk@wiproge.med.ge.com>
 * Rajeev V. Pillai    <rajeevvp@yahoo.com>
 */
static void parse_minix(struct parsed_partitions *state, __hdle bdev,
                        unsigned int offset, unsigned int size, int origin)
{
#ifdef CONFIG_MINIX_SUBPARTITION
    Sector sect;
    unsigned char *data;
    struct partition *p;

    data = read_dev_sector(bdev, offset, &sect);
    if (!data)
    {
        return;
    }

    p = (struct partition *)(data + 0x1be);

    /* The first sector of a Minix partition can have either
     * a secondary MBR describing its subpartitions, or
     * the normal boot sector. */
    if (msdos_magic_present(data + 510) &&
        SYS_IND(p) == MINIX_PARTITION)   /* subpartition table present */
    {
        int i;

        __log(" %s%d: <minix:", state->name, origin);
        for (i = 0; i < MINIX_NR_SUBPARTITIONS; i++, p++)
        {
            if (state->next == state->limit)
            {
                break;
            }
            /* add each partition in use */
            if (SYS_IND(p) == MINIX_PARTITION)
                put_partition(state, state->next++,
                              START_SECT(p), NR_SECTS(p));
        }
        __log(" >");
    }
    put_dev_sector(bdev, sect);
#endif /* CONFIG_MINIX_SUBPARTITION */
}

static struct
{
    unsigned char id;
    void (*parse)(struct parsed_partitions *, __hdle,
                  unsigned int, unsigned int, int);
} subtypes[] =
{
    {FREEBSD_PARTITION,     parse_freebsd   },
    {NETBSD_PARTITION,      parse_netbsd    },
    {OPENBSD_PARTITION,     parse_openbsd   },
    {MINIX_PARTITION,       parse_minix     },
    {UNIXWARE_PARTITION,    parse_unixware  },
    {SOLARIS_X86_PARTITION, parse_solaris_x86},
    {NEW_SOLARIS_X86_PARTITION, parse_solaris_x86},
    {0,                     0},
};

int check_msdos_partition(struct parsed_partitions *state, __hdle bdev)
{
    char *sect;
    unsigned char *data;
    struct partition *p;
    int slot;

    data = read_dev_sector(bdev, 0, &sect);
    if (!data)
    {
        return -1;
    }
    if (!msdos_magic_present(data + 510))
    {
        put_dev_sector(bdev, sect);
        return 0;
    }

    if (aix_magic_present(data, bdev))
    {
        put_dev_sector(bdev, sect);
        __log(" [AIX]");
        return 0;
    }

    /*
     * Now that the 55aa signature is present, this is probably
     * either the boot sector of a FAT filesystem or a DOS-type
     * partition table. Reject this in case the boot indicator
     * is not 0 or 0x80.
     */
    p = (struct partition *)(data + 0x1be);
    for (slot = 1; slot <= 4; slot++, p++)
    {
        if (p->boot_ind != 0 && p->boot_ind != 0x80)
        {
            put_dev_sector(bdev, sect);
            return 0;
        }
    }

#ifdef CONFIG_EFI_PARTITION
    p = (struct partition *)(data + 0x1be);
    for (slot = 1 ; slot <= 4 ; slot++, p++)
    {
        /* If this is an EFI GPT disk, msdos should ignore it. */
        if (SYS_IND(p) == EFI_PMBR_OSTYPE_EFI_GPT)
        {
            put_dev_sector(bdev, sect);
            return 0;
        }
    }
#endif
    p = (struct partition *)(data + 0x1be);

    /*
     * Look for partitions in two passes:
     * First find the primary and DOS-type extended partitions.
     * On the second pass look inside *BSD, Unixware and Solaris partitions.
     */

    state->next = 5;
    for (slot = 1 ; slot <= 4 ; slot++, p++)
    {
        unsigned int start = START_SECT(p);
        unsigned int size = NR_SECTS(p);
        if (!size)
        {
            continue;
        }
        if (is_extended_partition(p))
        {
            /* prevent someone doing mkfs or mkswap on an
               extended partition, but leave room for LILO */
            put_partition(state, slot, start, size == 1 ? 1 : 2);
            __log(" <");
            parse_extended(state, bdev, start, size);
            __log(" >");
            continue;
        }
        put_partition(state, slot, start, size);
        if (SYS_IND(p) == LINUX_RAID_PARTITION)
        {
            state->parts[slot].flags = 1;
        }
        if (SYS_IND(p) == DM6_PARTITION)
        {
            __log("[DM]");
        }
        if (SYS_IND(p) == EZD_PARTITION)
        {
            __log("[EZD]");
        }
    }

    __log("");

    /* second pass - output for each on a separate line */
    p = (struct partition *)(0x1be + data);
    for (slot = 1 ; slot <= 4 ; slot++, p++)
    {
        unsigned char id = SYS_IND(p);
        int n;

        if (!NR_SECTS(p))
        {
            continue;
        }

        for (n = 0; subtypes[n].parse && id != subtypes[n].id; n++)
            ;

        if (!subtypes[n].parse)
        {
            continue;
        }
        subtypes[n].parse(state, bdev, START_SECT(p), NR_SECTS(p), slot);
    }
    put_dev_sector(bdev, sect);
    return 1;
}

struct parsed_partitions *check_partition(__hdle bdev)
{
    struct parsed_partitions *state;
    int res = 0;

    state = malloc(sizeof(struct parsed_partitions));
    if (!state)
    {
        return NULL;
    }

    state->name[0] = 'p';
    state->name[1] = '\0';
    state->limit = MAX_PART;

    memset(&state->parts, 0, sizeof(state->parts));
    res = check_msdos_partition(state, bdev);
    if (res > 0)
    {
        return state;
    }

    free(state);
    return NULL;
}