/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_upcase.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system upcase handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:28:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __EXFAT_UPCASE_H__
#define __EXFAT_UPCASE_H__

int     exfat_setup_upcase(struct super_block *sb, u32 checksum,
                           u32 clusnr, u64 i_size);
void    exfat_free_upcase(struct exfat_sb_info *sbi);
__wchar_t exfat_towupper(struct exfat_sb_info *sbi, __wchar_t wc);

#endif /* __EXFAT_UPCASE_H__ */
