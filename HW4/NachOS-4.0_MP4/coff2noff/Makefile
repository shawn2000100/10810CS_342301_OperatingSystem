# Makefile for:
#	coff2noff -- converts a normal MIPS executable into a Nachos executable
#
# This is a GNU Makefile.  It must be used with the GNU make program.
# At UW, the GNU make program is /software/gnu/bin/make.
# In many other places it is known as "gmake".
# You may wish to include /software/gnu/bin/ early in your command
# search path, so that you will be using GNU make when you type "make".
#
#  Use "make" to build the executable(s)
#  Use "make clean" to remove .o files
#  Use "make distclean" to remove all files produced by make, including
#     the executable
#
#
# Copyright (c) 1992-1996 The Regents of the University of California.
# All rights reserved.  See copyright.h for copyright notice and limitation 
# of liability and disclaimer of warranty provisions.
#
#  This file has been modified for use at Waterloo
#
#############################################################################
# Makefile.dep contains all machine-dependent definitions
# If you are trying to build coff2noff somewhere outside
# of the MFCF environment, you will almost certainly want
# to visit and edit Makefile.dep before doing so
#############################################################################
include Makefile.dep

CC=gcc
CFLAGS= $(HOSTCFLAGS) -DRDATA -m32
LD=gcc -m32
RM = /bin/rm
MV = /bin/mv

ifeq ($(hosttype),unknown)
buildtargets = unknownhost
else
buildtargets = coff2noff.$(hosttype)
endif

all: $(buildtargets)

# converts a COFF file to Nachos object format
coff2noff.$(hosttype): coff2noff.o
	$(LD) coff2noff.o -o coff2noff.$(hosttype)
	strip coff2noff.$(hosttype)

clean:
	$(RM) -f coff2noff.o

distclean: clean
	$(MV) coff2noff.c temp.c
	$(RM) -f coff2noff.*
	$(MV) temp.c coff2noff.c


unknownhost:
	@echo Host type could not be determined.
	@echo make is terminating
	@echo If you are on an MFCF machine, contact the instructor
	@echo to report this problem
	@echo Otherwise, edit Makefile.dep and try again.
