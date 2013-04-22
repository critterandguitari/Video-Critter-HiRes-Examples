/******************************************************************************/
/*  graphics.c , version 1.0                                                       */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  Provides higher level graphics, drawing fonts */
/*                                                                            */
/*                                                                            */
/******************************************************************************/



#include "graphics.h"
#include "fonts.h"
#include "arial32.h"
#include "arial16.h"
#include "arial24.h"


extern unsigned int imageArray[40][128];   // defined in video.c

/******************************************************************************/
/*  High level graphic functions, independent of hardware                     */
/*      operate on imageArray                                                 */
/******************************************************************************/


void put_pixel(unsigned int x, unsigned int y, unsigned int on){    unsigned int temp;
    
    x %= 160;
    y &= 0x7f;
    on &= 0xff;

        temp = imageArray[x / 4][y];
        temp &= ~(0xff << ((x & 0x3) * 8));
        imageArray[x / 4][y] = temp | ((on) << ((x & 0x3) * 8));

}

unsigned int get_pixel(unsigned int x, unsigned int y){
}

// fill screen with constant color
void fill_screen(unsigned int color){
    int i, j, temp;
    color &= 0xff;
    temp = (color << 24) | (color << 16) | (color << 8) | color;
    for (i = 0; i < 40; i++)
        for (j = 0; j < 128; j++)
            imageArray[i][j] = temp;
}

void invert_screen(void){
    int i, j;
    for (i = 0; i < 40; i++)
        for (j = 0; j < 128; j++)
            imageArray[i][j] = ~imageArray[i][j];  // just flip all the bits in imageArray

}

// function to draw 7 x 9 rectagnle.  x and y are from 0 - 15. ends up being a pixel between rectangles
void draw_square(unsigned int x, unsigned int y, unsigned int color){
    
    int i, j;    
        
     x &= 0xf;
     y &= 0xf;   
    for (i = 0; i < 9; i++){
        for (j = 0; j < 7; j++){
            put_pixel(i + (x * 10), j + (y * 8), color);
        }   
    }
}


unsigned int rgb(unsigned int rgb){
    rgb &= 0xff;            // force to 8 bit value
    
    // shift the rgb bits around to match R, G, B pins
    return ((rgb & 0x4) >> 2) | (((rgb & 0x20) >> 4)) | (((rgb & 0x80) >> 5)) |  (((rgb & 0x2) << 2)) |
           (((rgb & 0x10))) | (((rgb & 0x40) >> 1)) | (((rgb & 0x1) << 6)) | (((rgb & 0x8) << 4));
}

unsigned int put_char_arial32(unsigned char character, unsigned int y, unsigned int x, unsigned int color){
  int i;
  int j;
  int k;
  int charWidth;
  int charOffset;
  
  if (character == 32)
    return 8;
  
  character -= 33;

  charWidth = arial32Width[character + 1];
  charOffset = arial32Offset[character] * 4;

  for (i = 0; i < 4; i++){
    for (j = 0; j < 8; j++){
      for (k = 0; k < charWidth; k++){
        if ((arial32[charOffset + k + (i * charWidth)] >> j) & 0x01)
          put_pixel((y + k),  (x + (i * 8) + j), color);
        else
          put_pixel((y + k),  (x + (i * 8) + j), 0);
      }
    }
  }
  return charWidth;
}

unsigned int put_char_arial24(unsigned char character, unsigned int y, unsigned int x, unsigned int color){
  int i;
  int j;
  int k;
  int charWidth;
  int charOffset;

  if (character == 32)
    return 6;

  character -= 33;

  charWidth = arial24Width[character + 1];
  charOffset = arial24Offset[character] * 3;


  
  for (i = 0; i < 3; i++){
    for (j = 0; j < 8; j++){
      for (k = 0; k < charWidth; k++){
        if ((arial24[charOffset + k + (i * charWidth)] >> j) & 0x01)
          put_pixel((y + k), (x + (i * 8) + j), color);
        else
          put_pixel((y + k), (x + (i * 8) + j), 0);
      }
    }
  }
  return charWidth;
}

unsigned int put_char_arial16(unsigned char character, unsigned int y, unsigned int x, unsigned int color){
  int i;
  int j;
  int k;
  int charWidth;
  int charOffset;

  if (character == 32)
    return 4;

  character -= 33;

  charWidth = arial16Width[character + 1];
  charOffset = arial16Offset[character] * 2;


  
  for (i = 0; i < 2; i++){
    for (j = 0; j < 8; j++){
      for (k = 0; k < charWidth; k++){
        if ((arial16[charOffset + k + (i * charWidth)] >> j) & 0x01)
          put_pixel((y + k), (x + (i * 8) + j), color);
        else
          put_pixel((y + k), (x + (i * 8) + j), 0);
      }
    }
  }
  return charWidth;
}

// print horizontally
unsigned int put_char_small(unsigned char c, unsigned int y, unsigned int x, unsigned int color){
	int i, j;
	c -= 32;
	for (i = 0; i < 5; i++){
		for (j = 0; j < 8; j++){
		    if ((characters[(c * 5) + i] >> j) & 0x01)
			    put_pixel(y + i, x + j, color);
			else
                put_pixel(y + i, x + j, 0);
		}
	}
        return 5;
}

// bresenham line algorithm from http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html
// modified for video critter

void line(int x0, int y0, int x1, int y1, unsigned int color) {
        int dy = y1 - y0;
        int dx = x1 - x0;
        int stepx, stepy;
        int fraction;

        if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
        if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
        dy <<= 1;                                                  // dy is now 2*dy
        dx <<= 1;                                                  // dx is now 2*dx

        put_pixel( x0, y0, color);
        if (dx > dy) {
            fraction = dy - (dx >> 1);                         // same as 2*dy - dx
            while (x0 != x1) {
                if (fraction >= 0) {
                    y0 += stepy;
                    fraction -= dx;                                // same as fraction -= 2*dx
                }
                x0 += stepx;
                fraction += dy;                                    // same as fraction -= 2*dy
                put_pixel(x0, y0, color);
            }
        } else {
            int fraction = dx - (dy >> 1);
            while (y0 != y1) {
                if (fraction >= 0) {
                    x0 += stepx;
                    fraction -= dy;
                }
                y0 += stepy;
                fraction += dx;
                put_pixel(x0, y0, color);
            }
        }
}
    
// bresenham circle algorithm
void circle(unsigned int h, unsigned int k, unsigned int r, unsigned int color){

       int x=0;
       int y=r;
       int p=(3-(2*r));

       do
	  {
	     put_pixel((h+x),(k+y),color);
	     put_pixel((h+y),(k+x),color);
	     put_pixel((h+y),(k-x),color);
	     put_pixel((h+x),(k-y),color);
	     put_pixel((h-x),(k-y),color);
	     put_pixel((h-y),(k-x),color);
	     put_pixel((h-y),(k+x),color);
	     put_pixel((h-x),(k+y),color);

	     x++;

	     if(p<0)
		p+=((4*x)+6);

	     else
		{
		   y--;
		   p+=((4*(x-y))+10);
		}
	  }
       while(x<=y);
}
