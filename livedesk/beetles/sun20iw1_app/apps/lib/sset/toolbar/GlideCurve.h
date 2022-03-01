#ifndef __GLIDE_CURVE_H__
#define __GLIDE_CURVE_H__

typedef struct
{
    int screen;     // 屏幕宽度
    int fb_len;     // 非空白区域长度
    int fb_start;   // 非空白区域起始位置
    int frame;      // 单元格宽度
} __gcuv_para_t;


int glidecuv_init(__gcuv_para_t *para);
int glidecuv_start(int src);
int glidecuv_move(int dest, int src, int *array, int max_array_num);
int glidecuv_up(int dest, int src, int speed, int speed_direct, int *array, int max_array_num);
int glidecuv_stop(int dest, int src, int *array, int max_array_num);

#endif /* __GLIDE_CURVE_H__ */
