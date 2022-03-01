/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsunaligned.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: fast hashing routine for a long, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:39:19   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __HASH_H__
#define __HASH_H__

/* Fast hashing routine for a long.
   (C) 2002 William Lee Irwin III, IBM */

/*
 * Knuth recommends primes in approximately golden ratio to the maximum
 * integer representable by a machine word for multiplicative hashing.
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * These primes are chosen to be bit-sparse, that is operations on
 * them can use shifts and additions instead of multiplications for
 * machines where multiplications are slow.
 */
#define BITS_PER_LONG 32
#if BITS_PER_LONG == 32
/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME 0x9e370001UL
#elif BITS_PER_LONG == 64
/*  2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18 + 1 */
#define GOLDEN_RATIO_PRIME 0x9e37fffffffc0001UL
#else
#error Define GOLDEN_RATIO_PRIME for your wordsize.
#endif

static inline unsigned long hash_long(unsigned long val, unsigned int bits)
{
    unsigned long hash = val;

#if BITS_PER_LONG == 64
    /*  Sigh, gcc can't optimise this alone like it does for 32 bits. */
    unsigned long n = hash;
    n <<= 18;
    hash -= n;
    n <<= 33;
    hash -= n;
    n <<= 3;
    hash += n;
    n <<= 3;
    hash -= n;
    n <<= 4;
    hash += n;
    n <<= 2;
    hash += n;
#else
    /* On some cpus multiply is faster, on others gcc will do shifts */
    hash *= GOLDEN_RATIO_PRIME;
#endif

    /* High bits are more random, so use them. */
    return hash >> (BITS_PER_LONG - bits);
}

static inline unsigned long hash_ptr(void *ptr, unsigned int bits)
{
    return hash_long((unsigned long)ptr, bits);
}
#endif /* __HASH_H__ */
