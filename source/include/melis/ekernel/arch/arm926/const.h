/*
 * =====================================================================================
 *
 *       Filename:  const.h
 *
 *    Description:  const symbol definition for platform.
 *
 *        Version:  2.0
 *         Create:  2017-11-02 10:19:02
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-05-24 13:59:19
 *
 * =====================================================================================
 */

#ifndef _CONST_H_
#define _CONST_H_

#include <kconfig.h>

#define CPU_ARCH_UNKNOWN                0
#define CPU_ARCH_ARMv3                  1
#define CPU_ARCH_ARMv4                  2
#define CPU_ARCH_ARMv4T                 3
#define CPU_ARCH_ARMv5                  4
#define CPU_ARCH_ARMv5T                 5
#define CPU_ARCH_ARMv5TE                6
#define CPU_ARCH_ARMv5TEJ               7
#define CPU_ARCH_ARMv6                  8
#define CPU_ARCH_ARMv7                  9

#define ARM_CPU_PART_CORTEX_A5          0xC050
#define ARM_CPU_PART_CORTEX_A7          0xC070
#define ARM_CPU_PART_CORTEX_A8          0xC080
#define ARM_CPU_PART_CORTEX_A9          0xC090
#define ARM_CPU_PART_CORTEX_A15         0xC0F0

#define EPDK_DEBUG_LEVEL_NOCHECK        0            /* No run time checks are performed                             */
#define EPDK_DEBUG_LEVEL_CHECK_ALL      1            /* Parameter checks and consistency checks are performed        */
#define EPDK_DEBUG_LEVEL_LOG_ERRORS     2            /* Errors are recorded                                          */
#define EPDK_DEBUG_LEVEL_LOG_WARNINGS   3            /* Errors & Warnings are recorded                               */
#define EPDK_DEBUG_LEVEL_LOG_ALL        4            /* Errors, Warnings and Messages are recorded.                  */

#define EPDK_AUDIO_READY                1
#define EPDK_SPI_READY                  1
#define EPDK_AUTO_PLAY_MOVIE_ENABLE     0
#define EPDK_PLAY_VIDEO_AUDIO_ENABLE    0
#define EPDK_LOAD_FROM_SD               0
#define EPDK_ROOTFS_PATH                "f:\\rootfs\\"
#define EPDK_TRANS_KEY_LONGLEFT2LONGVADD        1
#define EPDK_USE_ONLY_ONE_SD                    1                      //多媒体卡和启动卡是否同一个
#define EPDK_USE_EBOOK_TEST_TVIN                1
#define EPDK_PLAY_AUDIO_ENABLE                  1

#define EPDK_ARCH_SUNI          1
#define EPDK_ARCH_SUNII         2
#define EPDK_ARCH_SUNIII        3
#define EPDK_ARCH_SUNIV         4
#define EPDK_ARCH               EPDK_ARCH_SUNIV

////CPU_ARM920T
//#define CSIZE                   14  // cache size as 2**CSIZE (16 K assumed)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     6  // set associativity = 2**NWAY (64 way)
//#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
//#define I7WAY                   26  // CP15 c7 way incrementer as 2**SSET
//#define I9WAY                   26  // CP15 c9 way incrementer as 2**SSET

////ARM922T
//#define CSIZE                   14  // cache size as 2**CSIZE (16 K assumed)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     6  // set associativity = 2**NWAY (64 way)
//#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
//#define I7WAY                   26  // CP15 c7 way incrementer as 2**SSET
//#define I9WAY                   26  // CP15 c9 way incrementer as 2**SSET

////CPU_AC320
//#define ICSIZE                  15  // icache size as 2**CSIZE (32 K assumed)
//#define DCSIZE                  14  // dcache size as 2**CSIZE (16 K assumed)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     2  // set associativity = 2**NWAY (4 way)
//#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
//#define I7WAY                   30  // CP15 c7 way incrementer as 2**IWAY

////CPU_ARM940T
//#define CSIZE                   12  // cache size as 2**CSIZE (4K)
//#define CLINE                    4  // cache line size in bytes as 2**CLINE
//#define NWAY                     6  // set associativity = 2**NWAY (64 way)
//#define I7SET                    4  // CP15 c7 set incrementer = 2**ISET
//#define I7WAY                   26  // CP15 c7 way incrementer = 2**IWAY
//#define I9WAY                   0   // CP15 c9 way incrementer = 2**IWAY

////CPU_ARM946E_S
//#define CSIZE                   12  // cache size as 2**CSIZE (4 K assumed)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     2  // set associativity = 2**NWAY (4 way)
//#define I7SET                    4  // CP15 c7 set incrementer = 2**ISET
//#define I7WAY                   30  // CP15 c7 way incrementer = 2**IWAY
//#define I9WAY                    0  // CP15 c7 way incrementer = 2**IWAY

////CPU_ARM1022E
//#define CSIZE                   14  // cache size as 2**CSIZE (16 K)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     6  // set associativity = 2**NWAY (64 way)
//#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
//#define I7WAY                   26  // CP15 c7 way incrementer as 2**SSET
//#define I9WAY                   26  // CP15 c7 way incrementer = 2**IWAY

////CPU_ARM1026EJ_S
//#define CSIZE                   14  // cache size as 2**CSIZE (16 K assumed)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     2  // set associativity = 2**NWAY (4 way)
//#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
//#define I7WAY                   30  // CP15 c7 way incrementer as 2**IWAY

////CPU_SA_110
//#define CSIZE                   14  // cache size as 2**CSIZE (16 K)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     5  // set associativity = 2**NWAY (4 way)
//#define CleanAddressDcache      0x8000

////CPU_XSCALE
//#define CSIZE                   15  // cache size as 2**CSIZE (32 K)
//#define CLINE                    5  // cache line size in bytes as 2**CLINE
//#define NWAY                     5  // set associativity = 2**NWAY (32 way)
//#define MNWAY                    1  // set assoc mini D-cache = 2**NWAY (2 way)
//#define MCSIZE                  11  // mini cache size  as 2**CSIZE (2 K)
//#define CleanAddressDcache      0x8000  //(32K block 0x8000-0x10000)
//#define CleanAddressMiniDcache  0x10000 //(2K block 0x10000-0x10800)

//ARM926EJ_S
#ifdef CONFIG_SOC_SUN3IW1
#define ICSIZE                  15  // icache size as 2**CSIZE (32 K assumed)
#define DCSIZE                  14  // dcache size as 2**CSIZE (16 K assumed)
#define CLINE                    5  // cache line size in bytes as 2**CLINE
#define NWAY                     2  // set associativity = 2**NWAY (4 way)
#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
#define I7WAY                   30  // CP15 c7 way incrementer as 2**IWAY
#elif defined CONFIG_SOC_SUN3IW2
#define ICSIZE                  15  // icache size as 2**CSIZE (32 K assumed)
#define DCSIZE                  15  // dcache size as 2**CSIZE (32 K assumed)
#define CLINE                    5  // cache line size in bytes as 2**CLINE
#define NWAY                     2  // set associativity = 2**NWAY (4 way)
#define I7SET                    5  // CP15 c7 set incrementer as 2**ISET
#define I7WAY                   30  // CP15 c7 way incrementer as 2**IWAY
#elif defined CONFIG_SOC_SUN8IW19P1
#define CLINE                    5  // cache line size in bytes as 2**CLINE
#endif

#define DCSWAY                    (DCSIZE-NWAY)          //d cache size per way = 2**SWAY
#define DCNSET                    (DCSIZE-NWAY-CLINE)    //d cachelines per way = 2**NSET

#define ICSWAY                    (ICSIZE-NWAY)          //i cache size per way = 2**SWAY  
#define ICNSET                    (ICSIZE-NWAY-CLINE)    //i cachelines per way = 2**NSET

#endif /* #ifndef _CONST_H_ */
