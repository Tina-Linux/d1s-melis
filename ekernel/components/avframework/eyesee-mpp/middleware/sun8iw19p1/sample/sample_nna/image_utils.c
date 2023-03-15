/*
 * =====================================================================================
 *
 *       Filename:  image_utils.c
 *
 *    Description:  aw image utils
 *
 *        Version:  1.0
 *        Created:  07/23/2019 02:42:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jilinglin 
 *        Company:  allwinnertech.com
 *
 * =====================================================================================
 */
#include <unistd.h>
#include "image_utils.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"

void drawPoint(unsigned char *bits, int width, int depth, int x, int y, const unsigned char *color)
{
    int pix_depth = (depth < 3) ? depth : 3;
	for (int i = 0; i < pix_depth ; ++i) {
		bits[(y * width + x) * depth + i] = color[i];
	}
}

void drawLine(unsigned char *bits, int width, int depth,
			  int startX, int startY, int endX, int endY,const unsigned char *col)
{
	if (endX == startX) {
		if (startY > endY) {
			int a = startY;
			startY = endY;
			endY = a;
		}
		for (int y = startY; y <= endY; y++) {
			drawPoint(bits, width, depth, startX, y, col);
		}
	}
	else {
		float m = 1.0f * (endY - startY) / (endX - startX);
		int y = 0;
		if (startX > endX) {
			int a = startX;
			startX = endX;
			endX = a;
		}
		for (int x = startX; x <= endX; x++) {
			y = (int)(m * (x - startX) + startY);
			drawPoint(bits, width, depth, x, y, col);
		}
	}
}

void drawRectangle(unsigned char *bits, int width, int depth,
                    int x1, int y1, int x2, int y2, const unsigned char *col)
{
	if(x1 < 0 || x2<0 || y1<0 || y2<0){
		printf("xy pointer is error so don't draw x1:%d y1:%d x2:%d y2:%d\n",
				x1,y1,x2,y2);
		return;
	}
	drawLine(bits, width, depth, x1, y1, x2, y1, col);
	drawLine(bits, width, depth, x2, y1, x2, y2, col);
	drawLine(bits, width, depth, x2, y2, x1, y2, col);
	drawLine(bits, width, depth, x1, y2, x1, y1, col);
}


void draw_face_box(unsigned char *bits, int width, int depth,
                    draw_box* box, int box_num, int color_type)
{
	//some color
	const uint8_t red[3]    = { 255,   0,   0 };
	const uint8_t green[3]  = {   0, 255,   0 };
	const uint8_t blue[3]   = {   0,   0, 255 };
    const uint8_t aqua[3]   = {   0, 255, 255 };
    const uint8_t magenta[3]= { 255,   0, 255 };
    const uint8_t yellow[3] = { 255, 255,   0 };
    const uint8_t white[3]  = { 255, 255, 255 };
	const uint8_t green_haf[3] = { 0, 255/2, 0 };
	const uint8_t red_haf[3] = { 255/2, 0, 0 };
	const uint8_t blue_haf[3] = { 0, 0, 255/2 };
    const uint8_t yellow_haf[3] = { 255/2, 255/2, 0 };
    const uint8_t white_haf[3] = { 255/2, 255/2, 255/2 };


	const uint8_t * color;

	switch(color_type)
	{
        case COLOR_RED:
            color = (const uint8_t *)&red;
            break;
        case COLOR_GREEN:
            color = (const uint8_t *)&green;
            break;
        case COLOR_BLUE:
            color = (const uint8_t *)&blue;
            break;
        case COLOR_AQUA:
            color = (const uint8_t *)&aqua;
            break;
        case COLOR_MAGENTA:
            color = (const uint8_t *)&magenta;
            break;
        case COLOR_YELLOW:
            color = (const uint8_t *)&yellow;
            break;
        default:
            printf("corlor type is not support! use red.\n");
            color = (const uint8_t *)&red;

	}	


	for (int i = 0; i < box_num; i++) {
		drawRectangle(bits, width, depth, box[i].x1, box[i].y1,
			          box[i].x2,box[i].y2, color);
		drawRectangle(bits, width, depth, box[i].x1+1, box[i].y1+1,
			          box[i].x2-1,box[i].y2-1, color);
	
	}
}

void xrgb_to_c1(unsigned char *y_img, unsigned char *rgb_img, int w,int h)
{
	int x = 0, y = 0;
    unsigned int R,G,B,Y;
    int j=0;

    for(int i = 0; i < w*h*3;)      {
//        Y = 0.299 * R + 0.587 * G + 0.114 * B;
//        Y = 76/256 * R + 150/256 * G + 29/256 * B;
        R = rgb_img[i+0];
        G = rgb_img[i+1];
        B = rgb_img[i+2];
        R = R*76;
        G = G*150;
        B = B*29;
        Y = R+G+B;
        Y=Y>>8;
        y_img[j]=Y&0xFF;

        i += 3;
        j++;
	}
}

int EncodeImageToFile(const char *filepath, int width, int height, int channels, unsigned char *src)
{
	if(filepath == NULL || src == NULL){
		printf("filepath or src is null!\n");
		return -1;
	}
	if (!tje_encode_to_file(filepath, width, height, channels, true, src)) {
		printf("save JPEG fail.\n");
		return -1;
	}
    return 0;
}

unsigned char *DecodeImageFromFile(const char *filepath, int *Width, int *Height, int *Channels) 
{
	if(filepath == NULL){
		printf("filepath is null!\n");
		return NULL;
	}
    if(access(filepath,F_OK|R_OK) == -1){
		printf("file can not read!\n");
		return NULL;
	}
	return stbi_load(filepath, Width, Height, Channels, 0);
}
