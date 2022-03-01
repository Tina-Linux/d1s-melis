/*
 * =====================================================================================
 *
 *       Filename:  image_utils.h
 *
 *    Description:  image utils header file
 *
 *        Version:  1.0
 *        Created:  07/25/2019 02:46:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jilinglin
 *        Company:  allwinnertech.com
 *
 * =====================================================================================
 */
#ifndef _IMAGE_UTILS_H_
#define _IMAGE_UTILS_H_

typedef enum color_type {
   COLOR_RED = 0,
   COLOR_GREEN,
   COLOR_BLUE,
   COLOR_BLACK,
   COLOR_WHITE,
   COLOR_YELLOW,
   COLOR_AQUA,
   COLOR_MAGENTA,
   COLOR_HAFRED,
   COLOR_HAFGREEN,
   COLOR_HAFBLUE,
   COLOR_HAFYELLOW,
   COLOR_HAFWHITE,
} color_type;

typedef struct draw_box {
    int x1;   // left
    int y1;   // top
    int x2;   // right
    int y2;   // bottom                 
} draw_box;

void draw_face_box(unsigned char *bits, int width, int depth,
                   draw_box* box, int box_num, int color_type);
void drawRectangle(unsigned char *bits, int width, int depth,
                    int x1, int y1, int x2, int y2, const unsigned char *col);
void drawLine(unsigned char *bits, int width, int depth,
               int startX, int startY, int endX, int endY,const unsigned char *col);

void drawPoint(unsigned char *bits, int width, int depth,
                int x, int y, const unsigned char *color);
void xrgb_to_c1(unsigned char *y_img, unsigned char *rgb_img, int w,int h);
//encode rbg to jpeg and save into file
int EncodeImageToFile(const char *filename, int Width, int Height, int Channels, unsigned char *src);
//decode jpeg to rgb from file
unsigned char *DecodeImageFromFile(const char *filename, int *Width, int *Height, int *Channels);

#endif
