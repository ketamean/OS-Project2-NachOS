# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B ../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt scannersmain passengers scanners

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

scannersmain.o: scannersmain.c
	$(CC) $(CFLAGS) -c scannersmain.c
scannersmain: scannersmain.o start.o
	$(LD) $(LDFLAGS) start.o scannersmain.o -o scannersmain.coff
	../bin/coff2noff scannersmain.coff scannersmain

scanners.o: scanners.c
	$(CC) $(CFLAGS) -c scanners.c
scanners: scanners.o start.o
	$(LD) $(LDFLAGS) start.o scanners.o -o scanners.coff
	../bin/coff2noff scanners.coff scanners

passengers.o: passengers.c
	$(CC) $(CFLAGS) -c passengers.c
passengers: passengers.o start.o
	$(LD) $(LDFLAGS) start.o passengers.o -o passengers.coff
	../bin/coff2noff passengers.coff passengers
