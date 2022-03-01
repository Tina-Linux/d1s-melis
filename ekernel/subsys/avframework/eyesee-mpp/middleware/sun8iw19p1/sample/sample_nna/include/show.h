
#ifndef _SHOW_H_
#define _SHOW_H_


void lcd_put_pixel(unsigned char *img, int w, int h, int x, int y, unsigned int color);
void lcd_put_ascii(unsigned char *img, int w, int h, int x, int y, unsigned char c);
void lcd_puts(unsigned char *img, int w, int h, int x, int y, char *s);

int fb_init();
void fb_update(char *img,int w, int h);
void fb_close();
void xfbimg(char *img, int w, int h);


#endif
