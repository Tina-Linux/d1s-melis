/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-22
* Descript: Mini rom image file system structure.
* Update  : date                auther      ver     notes
*           2011-3-22 18:56:16  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MIN_FS_H__
#define __MIN_FS_H__

//Minfs spec
#define MINFS_MAGIC_LEN     6
#define MINFS_MAGIC         "MINFS"
#define MINFS_VERSION       0x100
#define MINFS_NAME_LEN      255         //the max length of minfs file name
#define MINFS_NAME_ALIGN    4           //minfs file name aligned by
#define MINFS_DATA_ALIGN    4           //minfs file data aligned by
#define MINFS_DENTRY_ALIGN  4           //minfs dentry aligned by
#define MINFS_SECTOR_LEN    512         //minfs sector size

//header of mini file system,
//byte size : 512.
typedef struct __MINFS_HDR
{
    __u8    Magic[MINFS_MAGIC_LEN];     //magic of mini file system
    __u16   Version;                    //version number of mini file system
    __u32   RootDirOffset;              //byte offset of root directory
    __u32   RootDirSize;                //byte size of root directory
    __u32   DEntryNum;                  //the  number of directory entry
    __u32   DEntryLen;                  //byte length of directory entry
    __u32   FDataLen;                   //byte size of file data area
    __u32   Size;                       //byte size of mini file system image
    __u8    Reserved[480];              //reserved area
} __minfs_hdr_t;

//length of minfs header, must be 512.
#define MINFS_HDR_LEN   (sizeof(__minfs_hdr_t))

//directory entry of mini file system,
//name and extent is the meta stream of dentry,
//the layout of name and extent : |name ||padding||extent|,
//the padding mainly for name aligned by 4byte.
typedef struct __MINFS_DENTRY
{
    __u32   Offset;                   //byte offset of directory entry data
    __u32   Size;                     //byte size of directory entry data
    __u32   UnPackSize;               //byte size of uncompressed directory entry data
    __u16   RecLen;                   //byte size of this dentry
    __u16   Attribute;                //attribute bits
    __u16   NameLen;                  //byte size of dentry name
    __u16   ExtentLen;                //byte offset of dentry extent data
    __u8    MetaData[MINFS_NAME_LEN]; //meta data of dentry, name + extent
} __minfs_dentry_t;

//length of minfs dentry
#define MINFS_DENTRY_LEN    (sizeof(__minfs_dentry_t) - MINFS_NAME_LEN - 1)

//attributes of Minfs directory entey
#define MINFS_ATTR_DIR          1       //directory
#define MINFS_ATTR_MODULE       2       //valid module file of melis system
#define MINFS_ATTR_COMPRESS     4       //file data been compessed
//...
//module of melis system : driver,module,plugs

//extent entry of elf file,
//the information is extraceted from elf section.
typedef struct __MINFS_SECTION_HDR
{
    __u32   Offset;                     //byte offset of this section
    __u32   RecSize;                    //byte size of this section in image
    __u32   RecUnPackSize;              //byte size of uncompressed in image
    __u32   Size;                       //byte size of this section data
    __u32   VAddr;                      //virtual address of this section
    __u32   Type;                       //type of this section
    __u32   Flags;                      //flags of this section
    __u32   Attribute;                  //Attribute bits of this section
} __minfs_section_hdr_t;

//length of minfs section header.
#define MINFS_SECTION_HDR_LEN   (sizeof(__minfs_section_hdr_t))

//attributes of minfs elf section header
#define MINFS_SECTION_ATTR_MAGIC        1   //melis system magic section
#define MINFS_SECTION_ATTR_COMPRESS     2   //section data been compessed
//...

#endif  // __MIN_FS_H__
