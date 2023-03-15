/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_math_util.h
 *
 * Copyright (c) 2007-2022 Allwinnertech Co., Ltd.
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ISP_MATH_UTIL_H_
#define _ISP_MATH_UTIL_H_

#include <sunxi_hal_common.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../include/isp_type.h"
#include "matrix.h"
#include <math.h>

#define min_t(type, a, b) ({			\
	type __a = (a);				\
	type __b = (b);				\
	__a < __b ? __a : __b;			\
})

#define max_t(type, a, b) ({			\
	type __a = (a);				\
	type __b = (b);				\
	__a > __b ? __a : __b;			\
})

#define clamp(val, min, max) ({			\
	typeof(val) __val = (val);		\
	typeof(min) __min = (min);		\
	typeof(max) __max = (max);		\
	__val = __val < __min ? __min : __val;	\
	__val > __max ? __max : __val;		\
})

#define clamp_t(type, val, min, max) ({		\
	type __val = (val);			\
	type __min = (min);			\
	type __max = (max);			\
	__val = __val < __min ? __min : __val;	\
	__val > __max ? __max : __val;		\
})

#define roundup(x, y) (					\
{							\
	const typeof(y) __y = y;			\
	(((x) + (__y - 1)) / __y) * __y;		\
}							\
)

#define square(x)	((x) * (x))
#define clear(x)	memset (&(x), 0, sizeof (x))

#define div_round_up(num, denom)	(((num) + (denom) - 1) / (denom))
#define div_round(num, denom)	(((num) + ((denom) >> 1)) / (denom))

#define Q4		16
#define Q8		256
#define Q10		1024

#define SHIFT_Q4	4
#define SHIFT_Q8	8
#define SHIFT_Q10	10

#define R_SHIFT_N(x,n)		(((x) + (1 << (n-1))) >> (n))
#define L_SHIFT_N(x,n)		((x) << (n))
#define IS_BETWEEN(x,a,b)	(((x) >= (a)) && ((x) <= (b)))

#define N_ROUND(x,y)		(((x) / (y)) * (y))

#define ABS(x) abs(x)

void HorizontalMirror(int arr[64]);
void VerticalMirror(int arr[64]);

int ValueInterp(int curr, int x_low,int x_high,int y_low,int y_high);
void Conv(int u[],int v[],int w[], int m, int n);

int ArrayFindMinIndex(int array[], int num);
int ArrayFindSecondMinIndex(int array[], int num, int exclude);
int ArrayFindMaxIndex(int array[],int num);
int ArrayFindSecondMaxIndex(int array[], int num, int exclude);

int ArraySum(int *array,int len);
int ArrayStdVar(int x[], int n);

void ShellSort(int *a,int *sub, int n);

int SqrtM(unsigned int a);

int RoundQ4(int x);

#if 0
struct matrix {
	float value[3][3];
};

void matrix_zero(struct matrix *m);
void matrix_invert(struct matrix *m);
void matrix_multiply(struct matrix *a, const struct matrix *b);
void matrix_float_to_s2q8(HW_S8Q8 out [3][3], const struct matrix *in);
void matrix_float_to_s4q8(HW_S8Q8 out[3][3], const struct matrix *in);
void matrix_float_to_s8q8(HW_S8Q8 out[3][3], const struct matrix *in);

void spline(double x[], double y[], int n, double yp1,double ypn, double y2[]);
void splint(double xa[], double ya[], double y2a[], int n, double x, double *y);
void spline_interp_u16(unsigned short x[], unsigned short y[], int n,
				unsigned short x2[], unsigned short y2[], int n2);
#endif
#endif /*_ISP_MATH_UTIL_H_*/

