#include <stdlib.h>

#include "cache_hash.h"
#include <log.h>

#include <rtthread.h>
#include <rtdebug.h>

#include <string.h>

#define NEXT_ELEMENT_ADDR(table, element) ((void *)(((unsigned long)(element)) + (table)->next_element_offset))
#define NEXT_ELEMENT(table, element) ((void *)(*(unsigned long *)NEXT_ELEMENT_ADDR(table, element)))
#define SET_NEXT_ELEMENT(table, element, next_element) (*(unsigned long *)NEXT_ELEMENT_ADDR(table, element) = (unsigned long)(next_element))

struct cache_hash_table *cache_hash_init(unsigned long table_size, int next_element_offset,
        cache_entry_compare_func compare_func,
        cache_entry_hash_func hash_func)
{
    struct cache_hash_table *table;
    unsigned int i;

    if (compare_func == NULL || hash_func == NULL)
    {
        return NULL;
    }

    table = (struct cache_hash_table *)malloc(sizeof(struct cache_hash_table));
    if (table == NULL)
    {
        return NULL;
    }
    memset(table, 0, sizeof(struct cache_hash_table));

    table->table_list = (struct cache_hash_element **)malloc(sizeof(void *) * table_size);
    if (table->table_list == NULL)
    {
        free(table);
        return NULL;
    }
    memset(table->table_list, 0, sizeof(void *) * table_size);

    for (i = 0; i < table_size; i++)
    {
        table->table_list[i] = NULL;
    }

    table->table_size = table_size;
    table->next_element_offset = next_element_offset;
    table->flags = 0;
    table->cnt_elements = 0;
    table->compare_func = compare_func;
    table->hash_func = hash_func;

    return table;
}

int cache_hash_deinit(struct cache_hash_table *table)
{
    if (table == NULL)
    {
        return -1;
    }

    free(table->table_list);
    free(table);

    return 0;
}

int cache_hash_insert(struct cache_hash_table *table, void *element)
{
    unsigned long hash;

    if (table == NULL || element == NULL)
    {
        return -1;
    }

    hash = table->hash_func(element, NULL, table->table_size);
    SET_NEXT_ELEMENT(table, element, table->table_list[hash]);
    table->table_list[hash] = (struct cache_hash_element *)element;
    table->cnt_elements++;

    return 0;
}

int cache_hash_remove(struct cache_hash_table *table, void *target_element)
{
    unsigned long hash = table->hash_func(target_element, NULL, table->table_size);
    void *element, *lastElement = NULL;

    for (element = table->table_list[hash]; element != NULL;
         lastElement = element, element = NEXT_ELEMENT(table, element))
    {
        if (element == target_element)
        {
            if (lastElement != NULL)
            {
                SET_NEXT_ELEMENT(table, lastElement, NEXT_ELEMENT(table, element));
            }
            else
            {
                table->table_list[hash] = (struct cache_hash_element *)NEXT_ELEMENT(table, element);
            }
            table->cnt_elements--;
            return 0;
        }
    }
    return -1;
}

void *cache_hash_lookup(struct cache_hash_table *table, const void *key)
{
    void *element;
    unsigned long hash;

    if (table == NULL || key == NULL)
    {
        return NULL;
    }

    hash = table->hash_func(NULL, key, table->table_size);

    for (element = table->table_list[hash]; element != NULL; element = NEXT_ELEMENT(table, element))
    {
        if (table->compare_func(element, key) == 0)
        {
            return element;
        }
    }
    return NULL;
}
