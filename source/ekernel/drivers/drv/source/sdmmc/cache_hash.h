#ifndef __CACHE_HASH_H__
#define __CACHE_HASH_H__

struct cache_hash_element
{
    void *element;
};

typedef int (* cache_entry_compare_func)(void *element, const void *key);
typedef unsigned long (* cache_entry_hash_func)(void *element, const void *key, unsigned long range);

struct cache_hash_table
{
    struct cache_hash_element **table_list;
    int     next_element_offset;
    int     cnt_elements;
    int     flags;
    unsigned long table_size;
    cache_entry_compare_func compare_func;
    cache_entry_hash_func hash_func;
};

typedef struct cache_hash_table cache_hash_table;

#ifdef __cplusplus
extern "C"
{
#endif

struct cache_hash_table *cache_hash_init(unsigned long table_size,
        int next_element_offset,
        cache_entry_compare_func compare_func,
        cache_entry_hash_func hash_func);

int cache_hash_deinit(struct cache_hash_table *table);
int cache_hash_insert(struct cache_hash_table *table, void *element);
int cache_hash_remove(struct cache_hash_table *table, void *element);
void *cache_hash_lookup(struct cache_hash_table *table, const void *key);

#ifdef __cplusplus
}
#endif

#endif  /* __CACHE_HASH_H__ */
