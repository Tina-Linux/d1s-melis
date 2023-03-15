/*-------------------------------------------------------------------------*/
/*
 * little endian
 *
 *-------------------------------------------------------------------------*/

#ifndef __ENDIAN_H
#define __ENDIAN_H

typedef uint16_t __le16;
typedef uint16_t __be16;
typedef uint32_t __le32;
typedef uint32_t __be32;
typedef uint64_t __le64;
typedef uint64_t __be64;

/*-------------------------------------------------------------------------*/

static inline uint16_t __swab16(uint16_t x)
{
	uint16_t __x = x;

	return (uint16_t)(
			 (((uint16_t)(__x)&(uint16_t)0x00ffU) << 8) |
			 (((uint16_t)(__x)&(uint16_t)0xff00U) >> 8));

}

static inline uint32_t __swab32(uint32_t x)
{
	uint32_t __x = (x);

	return (uint32_t)(
			 (((uint32_t)(__x)&(uint32_t)0x000000ffUL) << 24) |
			 (((uint32_t)(__x)&(uint32_t)0x0000ff00UL) <<  8) |
			 (((uint32_t)(__x)&(uint32_t)0x00ff0000UL) >>  8) |
			 (((uint32_t)(__x)&(uint32_t)0xff000000UL) >> 24));
}

static inline uint64_t __swab64(uint64_t x)
{
	uint64_t __x = (x);

	return (uint64_t)(
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x00000000000000ffULL) << 56) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x000000000000ff00ULL) << 40) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x0000000000ff0000ULL) << 24) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x00000000ff000000ULL) <<  8) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x000000ff00000000ULL) >>  8) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x0000ff0000000000ULL) >> 24) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0x00ff000000000000ULL) >> 40) |
			 (uint64_t)(((uint64_t)(__x)&(uint64_t)0xff00000000000000ULL) >> 56));
}
/*-------------------------------------------------------------------------*/

/**
 * __swab16p - return a byteswapped 16-bit value from a pointer
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline uint16_t __swab16p(const uint16_t *p)
{
	return __swab16(*p);
}

/**
 * __swab32p - return a byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline uint32_t __swab32p(const uint32_t *p)
{
	return __swab32(*p);
}

/**
 * __swab64p - return a byteswapped 64-bit value from a pointer
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline uint64_t __swab64p(const uint64_t *p)
{
	return __swab64(*p);
}
/*-------------------------------------------------------------------------*/

/**
 * __swab16s - byteswap a 16-bit value in-place
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline void __swab16s(uint16_t *p)
{
	*p = __swab16p(p);
}

/**
 * __swab32s - byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline void __swab32s(uint32_t *p)
{
	*p = __swab32p(p);
}

/**
 * __swab64s - byteswap a 64-bit value in-place
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline void __swab64s(uint64_t *p)
{
	*p = __swab64p(p);
}

/*-------------------------------------------------------------------------*/

#define cpu_to_le64(x) ((uint64_t)(x))
#define le64_to_cpu(x) ((uint64_t)(x))
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))

#define cpu_to_be64(x) __swab64((x))
#define be64_to_cpu(x) __swab64((x))
#define cpu_to_be32(x) __swab32((x))
#define be32_to_cpu(x) __swab32((x))
#define cpu_to_be16(x) __swab16((x))
#define be16_to_cpu(x) __swab16((x))

#define cpu_to_le64p(x) (*(uint64_t *)(x))
#define le64_to_cpup(x) (*(uint64_t *)(x))
#define cpu_to_le32p(x) (*(uint32_t *)(x))
#define le32_to_cpup(x) (*(uint32_t *)(x))
#define cpu_to_le16p(x) (*(uint16_t *)(x))
#define le16_to_cpup(x) (*(uint16_t *)(x))

#define cpu_to_be64p(x) __swab64p((x))
#define be64_to_cpup(x) __swab64p((x))
#define cpu_to_be32p(x) __swab32p((x))
#define be32_to_cpup(x) __swab32p((x))
#define cpu_to_be16p(x) __swab16p((x))
#define be16_to_cpup(x) __swab16p((x))

#define cpu_to_le64s(x) do {} while (0)
#define le64_to_cpus(x) do {} while (0)
#define cpu_to_le32s(x) do {} while (0)
#define le32_to_cpus(x) do {} while (0)
#define cpu_to_le16s(x) do {} while (0)
#define le16_to_cpus(x) do {} while (0)

#define cpu_to_be64s(x) __swab64s((x))
#define be64_to_cpus(x) __swab64s((x))
#define cpu_to_be32s(x) __swab32s((x))
#define be32_to_cpus(x) __swab32s((x))
#define cpu_to_be16s(x) __swab16s((x))
#define be16_to_cpus(x) __swab16s((x))

/*-------------------------------------------------------------------------*/
/* only for Scsi2.c to use
*/
static inline uint16_t get_be16(uint8_t *buf)
{
	return ((uint16_t)buf[0] << 8) | ((uint16_t)buf[1]);
}

static inline uint32_t get_be32(uint8_t *buf)
{
	return ((uint32_t) buf[0] << 24) | ((uint32_t) buf[1] << 16)
		| ((uint32_t) buf[2] << 8) | ((uint32_t) buf[3]);
}

static inline void put_be16(uint8_t *buf, uint16_t val)
{
	buf[0] = (uint8_t)(val >> 8);
	buf[1] = (uint8_t)val;
}

static inline void put_be32(uint8_t *buf, uint32_t val)
{
	buf[0] = (uint8_t)(val >> 24);
	buf[1] = (uint8_t)(val >> 16);
	buf[2] = (uint8_t)(val >> 8);
	buf[3] = (uint8_t)val;
}

#endif /* __ENDIAN_H */
