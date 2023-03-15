/*-------------------------------------------------------------------------*/
/*
 * little endian
 *
 *-------------------------------------------------------------------------*/

#ifndef __BITOPS_H
#define __BITOPS_H

/*-------------------------------------------------------------*/
/* USB bit option */

#define BITS_PER_LONG		(32)
#define BITS_ALL_MASK		(0x1F)
#define BIT_MASK(nr)		((1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)

static inline void usb_set_bit(uint32_t bit, volatile uint32_t *addr)
{
	uint32_t mask;
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	addr += BIT_WORD(bit);
	mask = BIT_MASK(bit);

	cpu_sr = hal_spin_lock_irqsave(&lock);
	(*addr) |= mask;
	hal_spin_unlock_irqrestore(&lock, cpu_sr);
}

static inline void usb_clear_bit(uint32_t bit, volatile uint32_t *addr)
{
	uint32_t mask;
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	addr += BIT_WORD(bit);
	mask = BIT_MASK(bit);

	cpu_sr = hal_spin_lock_irqsave(&lock);
	(*addr) &= (~mask);
	hal_spin_unlock_irqrestore(&lock, cpu_sr);
}

static inline uint32_t usb_test_bit(uint32_t bit, volatile uint32_t *addr)
{
	uint32_t res;
	uint32_t mask;
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	addr += BIT_WORD(bit);
	mask = BIT_MASK(bit);

	cpu_sr = hal_spin_lock_irqsave(&lock);
	res = *addr;
	hal_spin_unlock_irqrestore(&lock, cpu_sr);

	return ((mask & res) != 0);
}

static inline uint32_t usb_test_and_clear_bit(uint32_t bit, volatile uint32_t *addr)
{
	uint32_t res;
	uint32_t mask;
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	addr += BIT_WORD(bit);
	mask = BIT_MASK(bit);

	cpu_sr = hal_spin_lock_irqsave(&lock);
	res = *addr;
	*addr = res & (~mask);
	hal_spin_unlock_irqrestore(&lock, cpu_sr);

	return ((mask & res) != 0);
}

/*-------------------------------------------------------------*/
/* USB atomic option */

static inline void usb_dec32(uint32_t *addr)
{
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	cpu_sr = hal_spin_lock_irqsave(&lock);
	*addr = *addr - 1;
	hal_spin_unlock_irqrestore(&lock, cpu_sr);
}

static inline void usb_inc32(uint32_t *addr)
{
	uint64_t cpu_sr;
	hal_spinlock_t lock;

	cpu_sr = hal_spin_lock_irqsave(&lock);
	*addr = *addr + 1;
	hal_spin_unlock_irqrestore(&lock, cpu_sr);
}

/*-------------------------------------------------------------*/

static inline void Usb_uint2str_dec(uint32_t input, char *str)
{
	char stack[11] = {0};
	int i;
	int j;

	if (input == 0) {
		str[0] = '0';
		str[1] = '\0';	// 'str' must end with '\0'
		return;
	}

	for (i = 0; input > 0; ++i) {
		stack[i] = input % 10 + '0';  // characters in reverse order are put in 'stack' .
		input /= 10;
	}  // at the end of 'for' loop, 'i' is the number of characters.

	for (--i, j = 0; i >= 0; --i, ++j) {
		str[j] = stack[i];
	}

	str[j] = '\0';	// 'str' must end with '\0'
}

/*-------------------------------------------------------------*/
/*
*             find_next_zero_bit
* Description:
*     在地址连续的n个int字节中，从指定bit位开始查找第一个为0的bit的位.
* Parameters:
*     addr    :   起始地址.
*     size    :   n个字节的总长度,(32 * n).
*     offset  :   开始查找的bit位.
* Return value:
*     查找成功，返回查找到的第一个为0的bit位。查找失败，则返回0xffffffff。
*/

#define FIND_NEXT_ZERO_BIT_MAX_SIZE	(128)
#define FIND_NEXT_ZERO_BIT_NOT_FOUND	(0xffffffff)

static inline uint32_t find_next_zero_bit(const volatile uint32_t *addr, uint32_t size, uint32_t offset)
{
	uint32_t *p = (uint32_t *)addr;
	uint32_t bit = FIND_NEXT_ZERO_BIT_NOT_FOUND;
	int k;	//字节中的偏移量
	int i, j;
	int n;	// int字节的个数
	int m;	//所在字节的位置
	n = size / 32;
	k = 32 - (offset % 32);
	m = offset / 32;

	// offset为0 ~ size-1
	if (size > FIND_NEXT_ZERO_BIT_MAX_SIZE || offset >= size || m > n) {
		return bit;
	}

	//从offsize位置向前查找，直到结尾
	for (j = 0; j < (n - m); j++) {
		if (j == 0)  // offsize所在的字节
		{
			for (i = 0; i < k; i++) {
				if (((*(p + m + j) >> ((offset % 32) + i))) & 0x01) {
					continue;
				} else {
					bit = (offset + i);
					return bit;
				}
			}
		} else {  // offsize所在字节的下j个字节
			for (i = 0; i < 32; i++) {
				if ((*(p + m + j) >> i) & 0x01) {
					continue;
				} else {
					//                  bit = (offset+(j*32)+i);
					bit = ((j * 32) + i);
					return bit;
				}
			}
		}
	}

	//从addr位置向前查找，直到offsize处停止
	for (j = 0; j <= m; j++) {
		if (j != m) {  //从addr位置向前查找
			for (i = 0; i < 32; i++) {
				if (((*(p + j) >> i)) & 0x01) {
					continue;
				} else {
					bit = i + (j * 32);
					return bit;
				}
			}
		} else {  // offsize所在的字节
			for (i = 0; i < (32 - k); i++) {
				if (((*(p + j) >> i)) & 0x01) {
					continue;
				} else {
					bit = i + (j * 32);
					return bit;
				}
			}
		}
	}

	return bit;
}

#endif /* __BITOPS_H */
