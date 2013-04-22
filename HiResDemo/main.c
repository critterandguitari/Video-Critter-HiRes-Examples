/******************************************************************************/
/*  main.c                                                        */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  demonstrates some simple graphics and sound on the TV */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include "LPC21xx.h"
#include "system.h"
#include "video.h"
#include "audio.h"
#include "graphics.h"

// some timing functions
void start_ms_timer(void);
unsigned int ms_time(void);


int main (void) {

    unsigned int i, j, k, a, b, c, d;
    
    unsigned int delayTime = 255;
    
    int x_loc = 0;
    int y_loc = 0;
    int color = 0;

    
    // Initialize the MCU
    Initialize();

    led_board(0);
    delay_ms(100);              // flash LEDs
    led_board(1);
    delay_ms(100);
    led_board(0);
    delay_ms(100);
    led_board(1);  

    video_init();               // initialize video and audio synthesis
    audio_init();    

    delay_ms(10);
    
    fill_screen(0);             // fill screen with constant color (black in this case)
    
    // draw a multi colored line across screen
    for (i = 10; i < 100; i++)
        put_pixel(i, 64, i);       // put pixel takes 3 arguments: x and y position, and color (0 -13)
                                   // color is an 8 bit number with this format: (bits 7 - 0) G0, R0, B0, G1, R1, B1, G2, R2
                                  
    delay_ms(3000);      
    fill_screen(0);
    
    // we use the draw square function (actually its a rectangle) to draw the color palette 
    for (x_loc = 0; x_loc < 16; x_loc++){
        for (y_loc = 0; y_loc < 16; y_loc++){
                draw_square(x_loc, y_loc, rgb(color));  // rgb is used to convert an 8 bit number in rgb format to
                                                            // a number that matches the rgb pinout 
                                                        //  8 bit format: (bits 7 - 0) B1, B0, G2, G1, G0, R2, R1, R0
            color++;   
        }
    }

    delay_ms(3000);    
    fill_screen(0);             
       
    // circle:  x, y location, radius, color
    circle(80, 64, 10, RED);   // graphics.h also has defines for 8 colors
    circle(80, 64, 30, GREEN);
    
    // draw line: x0, y0, x1, y1, color
    line(0, 0, 159, 127, YELLOW);
    line(0, 127, 159, 0, MAGENTA);

    
    delay_ms(3000);
    fill_screen(0);
    
    // print some characters.
    // there are 4 put_char functions in the graphics.c each
    // for a different size font.
    // Each function takes 4 arguments:  character to print, x location, y location, and color.
    // The three larger fonts are variable width, and return the charcters width. 
    //  It is convinient to have variable x_loc to keep track of position.
    x_loc = 40;                  // set initial position
    x_loc += put_char_arial32('V', x_loc ,4, RED);x_loc += 2;  // add 2 more for space between characters
    x_loc += put_char_arial32('i', x_loc ,4, GREEN);x_loc += 2; 
    x_loc += put_char_arial32('d', x_loc ,4, BLUE);x_loc += 2;
    x_loc += put_char_arial32('e', x_loc ,4, CYAN);x_loc += 2;
    x_loc += put_char_arial32('o', x_loc ,4, MAGENTA);x_loc += 2;   
    x_loc = 38;
    x_loc += put_char_arial32('I', x_loc ,40, YELLOW);x_loc += 2;
    x_loc += put_char_arial32('n', x_loc ,40, WHITE);x_loc += 2;
    x_loc += put_char_arial32('v', x_loc ,40, RED);x_loc += 2;
    x_loc += put_char_arial32('e', x_loc ,40, GREEN);x_loc += 2;
    x_loc += put_char_arial32('r', x_loc ,40, BLUE);x_loc += 2;
    x_loc += put_char_arial32('t', x_loc ,40, MAGENTA);x_loc += 2;
    
    delay_ms(2000);
    
    invert_screen();
    delay_ms(3000);
    invert_screen();
    
    delay_ms(2000);
    
    fill_screen(0);

    a=b=c=d=0;
    delayTime = 127;
    
    for(;;){
    
        audio_fill_buf();           // this is the fucntion that computes the audio
                                    // it fills a 512 sample buffer that gets output to the DAC
                                    // It needs to be called requarly, about every 30 ms, 
                                    //  or else the buffer will overrun
                                    
        // here is how we do a delay in such a way that we can continue to call audio_fill_buf()
        if (ms_time() > delayTime){   // check to see if event occurs
            start_ms_timer();         // reset timer
      
            // below is where the real application happens. 
            // four bars of rectangles are drawn.  their height depends on the 
            // frequency of each synth voice.  the sequence repeats and repeats and speeds up
            
            delayTime--;                // speed up each time thru
            delayTime &= 0x7f;          // delay time between 0 - 127
          
            
            // variables a, b, c and d control the height of the bar, 
            //  and the frequency of each synth voice
  
        
            // draw the four bars.  erase the bar if any of the frequencies return to 0
            if(!a){
                for (i = 0; i < 16; i++){
                    draw_square(0, i, 0);
                }
            }
            draw_square(0, 15 - a, rgb(0x8f));
            
            
            if(!b){
                for (i = 0; i < 16; i++){
                    draw_square(2, i, 0);
                }
            }
            draw_square(2, 15 - b, rgb(0xf8));
           
            if(!c){
                for (i = 0; i < 16; i++){
                    draw_square(4, i, 0);
                }
            }
            draw_square(4, 15 - c, rgb(0xc3));
            
            if(!d){
                for (i = 0; i < 16; i++){
                    draw_square(6, i, 0);
                }
            }
            draw_square(6, 15 - d, rgb(0x3F));
            
            a++;                
            b++;
            c++;
            d++;              
            
            a &= 0xf;    // a plays 16 ascending notes
            b &= 0x7;    // b plays 8
            c &= 0x3;    // c plays 4
            d &= 0x1;    // d plays 2
                        
            // this function controls the audio synth: voice number (0 - 3), frequency (Hz), and amplitude (0 - 255)
            synth_voice(0, a * 100, 255);
            synth_voice(1, b * 200,  255);
            synth_voice(2, c * 300,  255);
            synth_voice(3, d * 400,  255);
        }
    }	
        	
} // main()





// resets the ms clock
void start_ms_timer(void){
    T1TCR = 0x2;                        // stop and reset timer
    T1PR = 58982;                       // timer prescale for ms (58982400 clks/sec)/1000 = 58982.4,
                                        // we round down to 58982 wich is a little off. (about 24ms short in 1 hour)
    T1TCR = 0x1;                        // start timer                                      
}

// this function returns the current ms time since start_ms_timer() was called 
unsigned int ms_time(void){
    volatile unsigned int current_time = 0;
    current_time = T1TC;         
    return current_time;                                
}


