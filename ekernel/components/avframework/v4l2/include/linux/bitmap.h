#ifndef __LINUX_BITMAP_H
#define __LINUX_BITMAP_H

#include <string.h>

#include <linux/bitops.h>

/*
 * Bitmap operation
 */
#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

#define small_const_nbits(nbits) \
	(__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG)

static inline void bitmap_zero(unsigned long *dst, unsigned int nbits)
{
	if ((nbits) <= BITS_PER_LONG)
		*dst = 0UL;
	else {
		unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memset(dst, 0, len);
	}
}

static inline int bitmap_empty(const unsigned long *src, unsigned nbits)
{
	if (small_const_nbits(nbits))
		return ! (*src & BITMAP_LAST_WORD_MASK(nbits));

	return find_first_bit(src, nbits) == nbits;
}

static inline int __bitmap_intersects(const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int bits)
{
	unsigned int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap1[k] & bitmap2[k])
			return 1;

	if (bits % BITS_PER_LONG)
		if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
			return 1;
	return 0;
}

static inline int bitmap_intersects(const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	if (small_const_nbits(nbits))
		return ((*src1 & *src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;
	else
		return __bitmap_intersects(src1, src2, nbits);
}

static inline void bitmap_fill(unsigned long *dst, unsigned int nbits)
{
	unsigned int nlongs = BITS_TO_LONGS(nbits);
	if (!small_const_nbits(nbits)) {
		unsigned int len = (nlongs - 1) * sizeof(unsigned long);
		memset(dst, 0xff,  len);
	}
	dst[nlongs - 1] = BITMAP_LAST_WORD_MASK(nbits);
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src,
			unsigned int nbits)
{
	if (small_const_nbits(nbits))
		*dst = *src;
	else {
		unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memcpy(dst, src, len);
	}
}

static inline int bitmap_and(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	unsigned int k;
	unsigned int lim = nbits/BITS_PER_LONG;
	unsigned long result = 0;

	if (small_const_nbits(nbits))
		return (*dst = *src1 & *src2 & BITMAP_LAST_WORD_MASK(nbits)) != 0;

	for (k = 0; k < lim; k++)
		result |= (dst[k] = src1[k] & src2[k]);
	if (nbits % BITS_PER_LONG)
		result |= (dst[k] = src1[k] & src2[k] &
			   BITMAP_LAST_WORD_MASK(nbits));
	return result != 0;
}

static inline void bitmap_or(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(nbits);

	if (small_const_nbits(nbits))
		*dst = *src1 | *src2;
	else
		for (k = 0; k < nr; k++)
			dst[k] = src1[k] | src2[k];
}

static inline void bitmap_xor(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(nbits);

	if (small_const_nbits(nbits))
		*dst = *src1 ^ *src2;
	else
		for (k = 0; k < nr; k++)
			dst[k]= src1[k] ^ src2[k];
}

static inline int bitmap_andnot(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	unsigned int k;
	unsigned int lim = nbits/BITS_PER_LONG;
	unsigned long result = 0;

	if (small_const_nbits(nbits))
		return (*dst = *src1 & ~(*src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;

	for (k = 0; k < lim; k++)
		result |= (dst[k] = src1[k] & ~src2[k]);
	if (nbits % BITS_PER_LONG)
		result |= (dst[k] = src1[k] & ~src2[k] &
			   BITMAP_LAST_WORD_MASK(nbits));
	return result != 0;
}

static inline void bitmap_complement(unsigned long *dst, const unsigned long *src,
			unsigned int nbits)
{
	unsigned int k, lim = nbits/BITS_PER_LONG;

	if (small_const_nbits(nbits))
		*dst = ~(*src);
	else {
		for (k = 0; k < lim; ++k)
			dst[k] = ~src[k];

		if (nbits % BITS_PER_LONG)
			dst[k] = ~src[k];
	}
}

static inline int bitmap_equal(const unsigned long *src1,
			const unsigned long *src2, unsigned int nbits)
{
	unsigned int k, lim = nbits/BITS_PER_LONG;

	if (small_const_nbits(nbits))
		return !((*src1 ^ *src2) & BITMAP_LAST_WORD_MASK(nbits));
#ifdef CONFIG_S390
	if (__builtin_constant_p(nbits) && (nbits % BITS_PER_LONG) == 0)
		return !memcmp(src1, src2, nbits / 8);
#endif
	for (k = 0; k < lim; ++k)
		if (src1[k] != src2[k])
			return 0;

	if (nbits % BITS_PER_LONG)
		if ((src1[k] ^ src2[k]) & BITMAP_LAST_WORD_MASK(nbits))
			return 0;
}

static inline void bitmap_clear(unsigned long *map, unsigned int start, int len)
{
	unsigned long *p = map + BIT_WORD(start);
	const unsigned int size = start + len;
	int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

	while (len - bits_to_clear >= 0) {
		*p &= ~mask_to_clear;
		len -= bits_to_clear;
		bits_to_clear = BITS_PER_LONG;
		mask_to_clear = ~0UL;
		p++;
	}
	if (len) {
		mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
		*p &= ~mask_to_clear;
	}
}

static inline void bitmap_set(unsigned long *map, unsigned int start, int len)
{
	unsigned long *p = map + BIT_WORD(start);
	const unsigned int size = start + len;
	int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

	while (len - bits_to_set >= 0) {
		*p |= mask_to_set;
		len -= bits_to_set;
		bits_to_set = BITS_PER_LONG;
		mask_to_set = ~0UL;
		p++;
	}
	if (len) {
		mask_to_set &= BITMAP_LAST_WORD_MASK(size);
		*p |= mask_to_set;
	}
}

static inline int bitmap_full(const unsigned long *src, unsigned int nbits)
{
	if (small_const_nbits(nbits))
		return ! (~(*src) & BITMAP_LAST_WORD_MASK(nbits));

	return find_first_zero_bit(src, nbits) == nbits;
}

#endif /* __LINUX_BITMAP_H */
