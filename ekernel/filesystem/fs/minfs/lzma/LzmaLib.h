/* LzmaLib.h -- LZMA library interface
2009-04-07 : Igor Pavlov : Public domain */

#ifndef __LZMA_LIB_H
#define __LZMA_LIB_H

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LZMA_PROPS_SIZE 5

/*
RAM requirements for LZMA:
  for compression:   (dictSize * 11.5 + 6 MB) + state_size
  for decompression: dictSize + state_size
    state_size = (4 + (1.5 << (lc + lp))) KB
    by default (lc=3, lp=0), state_size = 16 KB.

LZMA properties (4 bytes) format
    Offset Size  Description
      0     1    lc, lp and pb in encoded form.
      1     3    dictSize (little endian).
*/

/*
LzmaUncompress
--------------
In:
  dest     - output data
  destLen  - output data size
  src      - input data
  srcLen   - input data size
Out:
  destLen  - processed output size
  srcLen   - processed input size
Returns:
  SZ_OK                - OK
  SZ_ERROR_DATA        - Data error
  SZ_ERROR_MEM         - Memory allocation arror
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

int LzmaUncompress(unsigned char *dest, size_t *destLen,
                   const unsigned char *src, SizeT *srcLen,
                   const unsigned char *props, size_t propsSize);

#ifdef __cplusplus
}
#endif

#endif
