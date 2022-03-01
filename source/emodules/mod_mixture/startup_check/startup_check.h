#ifndef __STARTUP_CHECK__H_
#define __STARTUP_CHECK__H_
#include <mod_audio.h>
#include <libc.h>
#include <typedef.h>
#include <log.h>

typedef struct __WAVE_HEADER
{
    __u32       uRiffFcc;       // four character code, "RIFF"
    __u32       uFileLen;       // file total length, don't care it
    __u32       uWaveFcc;       // four character code, "WAVE"
    __u32       uFmtFcc;        // four character code, "fmt "
    __u32       uFmtDataLen;    // Length of the fmt data (=16)
    __u16       uWavEncodeTag;  // WAVE File Encoding Tag
    __u16       uChannels;      // Channels: 1 = mono, 2 = stereo
    __u32       uSampleRate;    // Samples per second: e.g., 44100
    __u32       uBytesPerSec;   // sample rate * block align
    __u16       uBlockAlign;    // channels * bits/sample / 8
    __u16       uBitsPerSample; // 8 or 16
    __u32       uDataFcc;       // four character code "data"
    __u32       uSampDataSize;  // Sample data size(n)
} __wave_header_t;

void check_if_lowpower(void);

#endif
