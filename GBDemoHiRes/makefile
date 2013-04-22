NAME   = LPC2138_flash

CC      = arm-elf-gcc
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		= arm-elf-objdump

CFLAGS  = -I./ -c -fno-common -O2 -g -mcpu=arm7tdmi -nostartfiles 
AFLAGS  = -ahls -mapcs-32 -o crt.o
LFLAGS  =  -Map main.map -T LPC2138_flash.cmd 
CPFLAGS = -O ihex
ODFLAGS	= -x --syms

all: test

clean:
	-rm crt.lst *.o

test: main.out
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.out main.hex
	$(OD) $(ODFLAGS) main.out > main.dmp

main.out: crt.o main.o video.o system.o graphics.o audio.o LPC2138_flash.cmd 
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.out  crt.o main.o video.o audio.o system.o graphics.o 

crt.o: crt.s
	@ echo ".assembling"
	$(AS) $(AFLAGS) crt.s > crt.lst

main.o: main.c
	@ echo ".compiling"
	$(CC) $(CFLAGS) main.c
	
graphics.o: graphics.c 
	@ echo ".compiling graphics.c"
	$(CC) $(CFLAGS) graphics.c 

video.o: video.c 
	@ echo ".compiling video.c"
	$(CC) $(CFLAGS) video.c 

system.o: system.c
	@ echo ".compiling system.c"
	$(CC) $(CFLAGS) system.c 

audio.o: audio.c
	@ echo ".compiling audio.c"
	$(CC) $(CFLAGS) audio.c 
