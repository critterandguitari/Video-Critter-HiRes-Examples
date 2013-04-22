/******************************************************************************/
/*  audio.c                                                        */
/*  (c)2006 GPL, Owen Osborn, Critter and Guitari                             */
/******************************************************************************/
/*                                                                            */
/*  :  audio.c  8 bit lookup synth voices for Video Critter                       */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
           
#include "LPC21xx.h"                // register definitions for processor
#include "system.h"                 // system level stuff, interrupt stuff
#include "waves.h"                  // contains the lookup table



/*
    This is a scaling value used to convert phase increment to Hertz.  
    The phase accumulator is 24 bits, so for a desired frequency f, 
    the phase increment = f(2 ^ 24) / sr, where sr = 15734 (NTSC Line freq).  
    So this scalar is (2 ^ 24) / 15734 = 1066.3. we round down.
*/
             
#define FREQSCALE   1066               


extern unsigned int soundBuf[512];
extern unsigned int soundBufR;
extern unsigned int soundBufW;

        
int sample;                  // value that eventually gets output to buffer

static int phaseAccum[] = {0, 0, 0, 0};
static int phaseDelta[] = {0, 0, 0, 0};
static int level[] = {255, 255, 255, 255};

/* 
    This function enables analog out
*/ 
void audio_init(void){
    PINSEL1 |= 0x00080000;              // enable dac
}

/* 
    This is the function we call to adjust the synthesizer
       the parameters.
*/
void synth_voice(int voice, int freq, int amp){
    voice &= 3;
    phaseDelta[voice] = freq * FREQSCALE;
    level[voice] = amp;
}


/* 
    This is the main audio function.  it fills up the audio buffer.
 */ 
void audio_fill_buf(void)
{ 
    
    // fill up buffer until current read location
    while (soundBufW != soundBufR){
    
        
        /* add phase increment value to accumulator */
        phaseAccum[0] += phaseDelta[0]; 
        phaseAccum[1] += phaseDelta[1];
        phaseAccum[2] += phaseDelta[2];
        phaseAccum[3] += phaseDelta[3];

        /* use bits 16 - 23 of the phase accumulator to lookup sample value from wave table */
        sample  = triTable[(phaseAccum[0] >> 16) & 0x000000ff] * level[0];   
        sample += triTable[(phaseAccum[1] >> 16) & 0x000000ff] * level[1]; 
        sample += triTable[(phaseAccum[2] >> 16) & 0x000000ff] * level[2]; 
        sample += triTable[(phaseAccum[3] >> 16) & 0x000000ff] * level[3]; 

        
        /* adjust amplitude */
        sample >>= 8; 
        
        
        /* 
            left adjust sample.  we added 4 8bit waves together yielding a 10 bit wave
            DACR is 16 bits, so shift right 6
        */
        soundBuf[soundBufW] = sample << 6;
        soundBufW++;
        soundBufW &= 0x1ff;
    }
         
}
