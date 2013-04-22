/******************************************************************************/
/*  video.h                                                                   */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  simple color raster graphics using with the Video Critter HiRes        */
/*                                     LPC2138 + AD725 RGB encoder            */
/*                                                                            */
/******************************************************************************/

#include "video.h"            
#include "LPC21xx.h"                // register definitions for processor
#include "system.h"                 // system level stuff, interrupt stuff



#define SYNC_RATE 3631     

#define ZERO            0x100;
#define BLACK           0x200;
        
#define DELAY1	{asm volatile("nop");}
#define DELAY2 	{asm volatile("nop");asm volatile("nop");}
#define DELAY4	{asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");}
#define DELAY8	{asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");}
#define DELAY16 {asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");}
#define DELAY32 {asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");}
        
#define DELAY256 {DELAY32;DELAY32;DELAY32;DELAY32;DELAY32;DELAY32;DELAY32;DELAY32;}  

unsigned int soundBuf[512];         // audio buffer
unsigned int soundBufR = 0;
unsigned int soundBufW = 0;


int newLine = 0;
int lineCount = 0;
unsigned int imageVindex = 0;

unsigned int imageArray[40][128];

static int pixelCount = 0; 

int syncOn = ZERO; 
int syncOff = BLACK;       

static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));
void fiq_handler(void) __attribute__((interrupt("FIQ")));


void video_init(void){

    IODIR0 &= ~(1 << 29);      // vsync, just set to input

    PINSEL1 |= (1 << 10);  // pwm 5
    PINSEL1 |= ((1 << 24) | (1 << 25));    // match timer0. 2

    PWMPR = 0x00000000;            // Set Pre Scale to 0
    PWMPCR = (1 << 10) | (1 << 13);//0x1000; // Enable PWM4 Output (bit12)

    PWMMR0 = 4; // Max Value of Pulse Width
    PWMMR2 = 2; // PWM2 Match Value
    PWMMR5 = 2; // PWM5 Match Value

    PWMMCR = 0x00000002; // Reset PWMMTC when Matched with PWMMR0
    PWMLER = 0x7F; // Latch in all PWMMMR's
    PWMTCR = 0x02; // Reset PWMMTC, (bit1)
    PWMTCR = 0x09; // Counter Enable, PWM Mode Enabled(bit0, bit3) */

    IODIR0 |= (1 << 23);
    PINSEL2 &= ~(1 << 3);               // make sure P1.23:16 are gpio
    IODIR1 |= (0xff << 16);             // P1.23:16 output
    PINSEL1 |= 0x00080000;              // enable dac
    VICIntSelect &= ~0x10;              // timer 0 is IRQ
    VICIntEnable = 0x10;                // enable timer 0 interrupt
    VICVectCntl0 = 0x24;                //  use slot 0 for timer 0 interrupt 
    VICVectAddr0 = (unsigned int)timer0ISR;     // address of ISR for slot 0
    T0TCR = 0;                          // reset timer
    T0PR = 0;                           // timer prescale
    T0MR0 = SYNC_RATE;                // match at sampling rate
    T0MR1 = SYNC_RATE;
    T0MR2 = SYNC_RATE;
    T0MR3 = SYNC_RATE;
    
    T0EMR &= ~0xff0;
    T0EMR |= ZERO;
    
    T0MCR = 3;                          // interrupt and reset on match
    
    T0TCR = 1;                          // start timer
    enableIRQ();                        // enable interrupt
}

void video_stop(void){
    T0TCR = 0;                          // stop timer
    disableIRQ();                       // interrupts off
}



static void timer0ISR(void)
{ 

    T0MCR = 0;                          // don't interrupt or reset on match
    
    T0MR0 = 250;                        // match at next event
    T0MR1 = 250;
    T0MR2 = 250;
    T0MR3 = 250;   
    T0EMR &= ~0xff0;
    T0EMR |= syncOff;

   DACR = soundBuf[soundBufR];
    soundBufR++;
    soundBufR &= 0x1ff;

    while (T0TC < 251);

    if ((lineCount > 25) && (lineCount < 217)){

        // setup FIQ interrupt
        T0MR0 = 800;
        T0IR = 0xFF;                        // clear interrupts
        VICIntSelect = 0x10;              // timer 0 is FIQ
        T0MCR = 1;                         // interrupt but don't reset on match
        PCON = 1;                         // stop cpu              
    }
    
    lineCount++;
    imageVindex = lineCount - 25;
    imageVindex *= 2;
    imageVindex /= 3;
   // imageVindex >>= 1;
        
    // reset line
    T0MR0 = SYNC_RATE;                // match at sampling rate
    T0MR1 = SYNC_RATE;
    T0MR2 = SYNC_RATE;
    T0MR3 = SYNC_RATE;
        

    if ((lineCount >= 248) && (lineCount < 251)){  // invert sync for vertical
        
        syncOn = BLACK;
        syncOff = ZERO;
    }
    else {
        syncOn = ZERO;
        syncOff = BLACK;
    }
    
    T0EMR &= ~0xff0;
    T0EMR |= syncOn;
    
            // reset line
    if (lineCount==263)
        lineCount = 1;
    

    T0MCR = 3;                          // interrupt and reset on match
    
    T0IR = 0xFF;                        // clear interrupts
    VICVectAddr = 0;                    // update vic priorities
}


void fiq_handler(void){

    T0MCR = 0;                          // don't interrupt or reset on match
    for (pixelCount = 0; pixelCount < 40; pixelCount++){
            IOPIN1 = (imageArray[pixelCount][imageVindex] >> 0) << 16;DELAY2;DELAY2;
            IOPIN1 = (imageArray[pixelCount][imageVindex] >> 8) << 16;DELAY2;DELAY1;
            IOPIN1 = (imageArray[pixelCount][imageVindex] >> 16) << 16;DELAY2;DELAY1;
            IOPIN1 = (imageArray[pixelCount][imageVindex] >> 24)<< 16;DELAY2;
    }
    
    IOPIN1 = 0;//7 << 4;
    
    //T0EMR = 1;  // black
    VICIntSelect &= ~0x10;              // timer 0 is IRQ
    T0IR = 0xFF;                        // clear interrupts
}

