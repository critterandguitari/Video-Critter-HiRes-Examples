/******************************************************************************/
/*  graphics.c , version 1.1                                                       */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  Provides higher level graphics, drawing fonts */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#define RED     0x49
#define GREEN   0x92
#define BLUE    0x24
#define CYAN    0xB6
#define MAGENTA 0x6D
#define YELLOW  0xDB
#define BLACK   0
#define WHITE   0xff



void put_pixel(unsigned int on, unsigned int x, unsigned int y);
unsigned int get_pixel(unsigned int x, unsigned int y);
void fill_screen(unsigned int color);
void invert_screen(void);

unsigned int rgb(unsigned int rgb);

void draw_square(unsigned int x, unsigned int y, unsigned int color);

unsigned int put_char_arial32(unsigned char character, unsigned int x, unsigned int y, unsigned int color);

unsigned int put_char_arial24(unsigned char character, unsigned int x, unsigned int y, unsigned int color);

unsigned int put_char_arial16(unsigned char character, unsigned int x, unsigned int y, unsigned int color);

unsigned int put_char_small(unsigned char c, unsigned int x, unsigned int y, unsigned int color);

void line(int x0, int y0, int x1, int y1, unsigned int color);
    
void circle(unsigned int h, unsigned int k, unsigned int r, unsigned int color);

