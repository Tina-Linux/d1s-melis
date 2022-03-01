/*
 * =====================================================================================
 *
 *       Filename:  bcache.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2020-04-30 20:03:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef BCACHE_H
#define BCACHE_H

extern int flush_block_cache(int);
extern int discard_block_cache(void);
extern int block_cache_manager_init(int heap_size);
extern int block_cache_manager_deinit(void);

extern int sdmmc_cache_read(void *pBuffer, int block_num, int nBlockCount, int bsize, __hdle dev);

extern int sdmmc_cache_write(void *pBuffer, int block_num, int nBlockCount, int bsize, __hdle dev);

extern __u32 sdmmc_dev_phy_write(const void *pBuffer, __u32 blk, __u32 n, __hdle hDev);
extern __u32 sdmmc_dev_phy_read(void *pBuffer, __u32 blk, __u32 n, __hdle hDev);

#endif  /*BCACHE_H*/
