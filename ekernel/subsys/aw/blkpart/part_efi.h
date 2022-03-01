#ifndef _DISK_PART_EFI_H
#define _DISK_PART_EFI_H

#ifndef CONFIG_PARTITIONS_TABLE_SIZE
#define CONFIG_PARTITIONS_TABLE_SIZE 16
#endif
#ifndef CONFIG_PARTITIONS_LOGICAL_OFFSET
#define CONFIG_PARTITIONS_LOGICAL_OFFSET  64
#endif

#include <typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __packed
#define __packed __attribute__((packed))
#endif

typedef struct
{
    u8 b[16];
} efi_guid_t;

#define SUNXI_MBR_SIZE  (16 * 1024)
#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL

typedef u16 efi_char16_t;

typedef struct _gpt_header
{
    u64 signature;
    u32 revision;
    u32 header_size;
    u32 header_crc32;
    u32 reserved1;
    u64 my_lba;
    u64 alternate_lba;
    u64 first_usable_lba;
    u64 last_usable_lba;
    efi_guid_t disk_guid;
    u64 partition_entry_lba;
    u32 num_partition_entries;
    u32 sizeof_partition_entry;
    u32 partition_entry_array_crc32;
} __packed gpt_header;

typedef union _gpt_entry_attributes
{
    struct
    {
        u64 required_to_function: 1;
        u64 no_block_io_protocol: 1;
        u64 legacy_bios_bootable: 1;
        /* u64 reserved:45; */
        u64 reserved: 27;
        u64 user_type: 16;
        u64 ro: 1;
        u64 keydata: 1;
        u64 type_guid_specific: 16;
    } fields;
    unsigned long long raw;
} __packed gpt_entry_attributes;

#define PARTNAME_SZ    (72 / sizeof(efi_char16_t))
typedef struct _gpt_entry
{
    efi_guid_t partition_type_guid;
    efi_guid_t unique_partition_guid;
    u64 starting_lba;
    u64 ending_lba;
    gpt_entry_attributes attributes;
    efi_char16_t partition_name[PARTNAME_SZ];
} __packed gpt_entry;

#define  GPT_ENTRY_OFFSET        1024
#define  GPT_HEAD_OFFSET         512

#define GPT_TABLE_SIZE (CONFIG_PARTITIONS_TABLE_SIZE * 1024)
#define GPT_START_MAPPING (CONFIG_PARTITIONS_LOGICAL_OFFSET * 1024)
#define GPT_ADDRESS (GPT_START_MAPPING - GPT_TABLE_SIZE)

struct gpt_part
{
    char name[PARTNAME_SZ];
    u32 off_sects;
    u32 sects;
    int index;

    gpt_header *header;
    gpt_entry *entry;
};
struct gpt_part *first_gpt_part(void *gpt_buf);
struct gpt_part *next_gpt_part(struct gpt_part *part);

#define foreach_gpt_part(gpt_buf, gpt_part)                 \
    for (gpt_part = first_gpt_part(gpt_buf);                \
         gpt_part; gpt_part = next_gpt_part(gpt_part))

int gpt_part_cnt(void *gpt_buf);
int show_gpt_part(void *gpt_buf);

#ifdef __cplusplus
}
#endif

#endif    /* _DISK_PART_EFI_H */
