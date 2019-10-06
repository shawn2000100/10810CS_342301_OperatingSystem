# Copyright (c) 1992-1996 The Regents of the University of California.
# All rights reserved.  See copyright.h for copyright notice and limitation 
# of liability and disclaimer of warranty provisions.
#
# This is a GNU Makefile.  It must be used with the GNU make program.
# At UW, the GNU make program is /software/gnu/bin/make.
# In many other places it is known as "gmake".
# You may wish to include /software/gnu/bin/ early in your command
# search path, so that you will be using GNU make when you type "make".
#
# About this Makefile:
# --------------------
#
#  This Makefile is used to build the Nachos system, which includes
#   the MIPS machine simulation and a simple operating system.
#
#  There is a separate Makefile, in the "test" directory, that is
#   used to build the Nachos test programs (which run on the
#   simulated machine).
#
#  There are several "build" directories, one for each type
#   of machine in the MFCF computing environment
#   (build.solaris, build.sunos, and build.ultrix), as well
#   as a build directory for Linux (build.linux) and a generic
#   build directory (build.other) for those who wish to try
#   building Nachos on other platforms.
#
#  This Makefile appears to be located in all of the build directories.
#  If you edit it in one directory, the copies in all of the other
#  directories appear to change as well.  This is the desired behaviour,
#  since this file is machine independent.  (The file actually lives
#  in build.solaris, with symbolic links from the other build directories.)
#
#  The platform-dependent parts of make's instructions are located
#  in the file Makefile.dep.
#  There is a different Makefile.dep in each build directory.
#
#  If you are in the MFCF environment, you should not have to edit
#  the Makefile.dep files by hand.  Any changes to the make instructions
#  can be made in this file (see the instructions below) - they will
#  apply no matter where you build Nachos.
#  If you are not in the MFCF environment, e.g., if you are trying
#  to build Nachos on Linux at home, you will probably need
#  to edit Makefile.dep (in the appropriate build directory) to
#  customize the make procedure to your environment.
#
# How to build Nachos for the first time:
# ---------------------------------------
#
#   (1) Make sure than you are in the build directory for the
#	type of machine you are logged in to (the "host" machine):
#
#	host type	examples		build directory
# 	-----------	-----------		----------------
#
#	sparc/SunOS	cayley,napier,		build.sunos
#	(SunOS 4.1.3)	descartes
#
#	sparc/Solaris	picard.math,		build.solaris
#	(SunOS 5.x)	hermite.math,
#			markov.math,
#			hypatia.math,
#			hume.math
#
#	mips/ULTRIX	cantor.math		build.ultrix
#	(ULTRIX 4.2)	noether.math
#
#	If you are not sure what type of machine you are on,
#	try the command "uname -a".  
#
#   (2) Type "make depend"
#		- this computes file dependencies and records them
#			at the end of the file Makefile.dep in 
#			your build directory.  Have a look...
#
#   (3) Type "make nachos"  (or just "make").
#               - make echos the commands it is executing, so that
#			you can observe its progress.  When the
#			build is finished, you should have an
#			executable "nachos" in the build directory.
#
#   (4) There is no 4th step.  You are done.  Try running "./nachos -u".
#
#
# How to Re-build Nachos after you have changed the code:
#--------------------------------------------------------
#
#     - The Nachos source code is located in the code subdirectories:
#	  threads, userprog, filesys, network, and lib.  You may
#         change the files in any of these directories, and you can
#         add new files and/or remove files. The "machine" subdirectory
#	  contains the hardware simulation (which is also part of
#	  Nachos.  You may look at it, but
#	  you may not change it, except as noted in machine/machine.h
#     - When you want to re-make Nachos, always do it in the
#	  "build" directory that is appropriate for the machine
#	  type that you are running on.  
#	DO NOT TRY TO MAKE NACHOS IN THE SOURCE CODE DIRECTORIES.
#
#     - IF all you have done is changed C++ code in existing files
#	(since the last time you made Nachos in this build directory),
#	THEN all you need to do to re-make Nachos is to type
#
#		"make nachos"
#
#	in the build directory.
#
#     - IF you have done any of the following since the last build in
#	  this directory:
#		added new .cc files or new .h files
#		added or deleted #include's from existing files
#	THEN
#		you must do
#		"make depend"
#		followed by
#		"make nachos"
#
#	in the build directory.
#
#       Note that is is always safe to do "make depend" followed by
# 	"make nachos", so if you are not sure what changes you have
#	made, do "make depend".
#
#     - IF you have added new files (.cc or .h) since the last build,
#	you should edit this Makefile before running "make depend"
# 	and "make nachos".

#	For new .h files, simply update the appropriate "_H" list below.
#       For example, if you create a file called
#       "bigfile.h" in the filesys subdirectory, you should add
#       "../filesys/bigfile.h" to FILESYS_H, which is defined below

#	For new .cc files, update the appropriate "_C" and "_O" lists.
#       For example, if you create a file called "filetable.cc" in
#	the directory "userprog", you should add
#	"../userprog/filetable.cc" to USERPROG_C,
#       and you should add "filetable.o" to USERPROG_O.
#       Note that the entry in the "_C" list includes the subdirectory
#	name, while the entry on the "_O" list does not.
#
#  Some Important Notes:
#  ---------------------
#
#       *  You can clean up all of the .o and other files left behind
#	   by make by typeing "make clean" in the build directory.
#	*  You can clean up .o and other files, as well as the nachos
#	   executable, DISK, core, SOCKET, and other files by typing
#	   make "distclean"
#
#	These are good ways to save space, but the next build that
#	you do after cleaning will take longer than usual, since
#	much of the stuff you cleaned will need to be rebuilt.
#
#       *  When you build Nachos on an ULTRIX machine (in build.ultrix),
#	   you will get lots of warning messages like this:
#
#	openfile.o: does not have gp tables for all it's sectons
#
#	   from the loader.  Ignore them.  Or better yet, figure out
#	   how to make them go away.
#
#  The Most Important Note:
#  -----------------------
#
#	* If "make" is behaving strangely and you cannot figure out
#	why, you should REBUILD the program FROM SCRATCH.
#	Yes, it is slow.
#	But, there are lots of little things that can go wrong, especially
#	with all of these different types of machines available.
#	Rebuilding from scratch at least gives you a known starting
#	place.  To rebuild from scratch, go to the appropriate
#	build directory and do:
#	
#		make distclean
#		make depend
#		make nachos
#
################################################################
#  READ THIS:   CONFIGURING NACHOS
#
# Change DEFINES (below) to
#   DEFINES = -DUSE_TLB -DFILESYS_STUB
# if you want the simulated machine to use its TLB
#
# If you want to use the real Nachos file system (based on
# the simulated disk), rather than the stub, remove
# the -DFILESYS_STUB from DEFINES.
#
# There is a a fix to the MIPS simulator to enable it to properly
# handle unaligned data access.  This fix is enabled by the addition
# of "-DSIM_FIX" to the DEFINES.  This should be enabled by default
# and eventually will not require the symbol definition
################################################################
DEFINES =  -DFILESYS_STUB -DRDATA -DSIM_FIX


#####################################################################
#
# You might want to play with the CFLAGS, but if you use -O it may
# break the thread system.  You might want to use -fno-inline if
# you need to call some inline functions from the debugger.

CFLAGS = -g -Wall -fwritable-strings $(INCPATH) $(DEFINES) $(HOSTCFLAGS) -DCHANGED
LDFLAGS =

#####################################################################
CPP=/lib/cpp
CC = g++
LD = g++
AS = as
RM = /bin/rm

INCPATH = -I../network -I../filesys -I../userprog -I../threads -I../machine -I../lib

PROGRAM = nachos

#
# Edit these lists as if you add files to the source directories.
# See the instructions at the top of the file for more information.
#

LIB_H = ../lib/bitmap.h\
	../lib/copyright.h\
	../lib/debug.h\
	../lib/hash.h\
	../lib/libtest.h\
	../lib/list.h\
	../lib/sysdep.h\
	../lib/utility.h

LIB_C = ../lib/bitmap.cc\
	../lib/debug.cc\
	../lib/hash.cc\
	../lib/libtest.cc\
	../lib/list.cc\
	../lib/sysdep.cc

LIB_O = bitmap.o debug.o libtest.o sysdep.o


MACHINE_H = ../machine/callback.h\
	../machine/interrupt.h\
	../machine/stats.h\
	../machine/timer.h\
	../machine/console.h\
	../machine/machine.h\
	../machine/mipssim.h\
	../machine/translate.h\
	../machine/network.h\
	../machine/disk.h

MACHINE_C = ../machine/interrupt.cc\
	../machine/stats.cc\
	../machine/timer.cc\
	../machine/console.cc\
	../machine/machine.cc\
	../machine/mipssim.cc\
	../machine/translate.cc\
	../machine/network.cc\
	../machine/disk.cc

MACHINE_O = interrupt.o stats.o timer.o console.o machine.o mipssim.o\
	translate.o network.o disk.o

THREAD_H = ../threads/alarm.h\
	../threads/kernel.h\
	../threads/main.h\
	../threads/scheduler.h\
	../threads/switch.h\
	../threads/synch.h\
	../threads/synchlist.h\
	../threads/thread.h

THREAD_C = ../threads/alarm.cc\
	../threads/kernel.cc\
	../threads/main.cc\
	../threads/scheduler.cc\
	../threads/synch.cc\
	../threads/synchlist.cc\
	../threads/thread.cc

THREAD_O = alarm.o kernel.o main.o scheduler.o synch.o thread.o

USERPROG_H = ../userprog/addrspace.h\
	../userprog/syscall.h\
	../userprog/synchconsole.h\
	../userprog/noff.h

USERPROG_C = ../userprog/addrspace.cc\
	../userprog/exception.cc\
	../userprog/synchconsole.cc

USERPROG_O = addrspace.o exception.o synchconsole.o

FILESYS_H =../filesys/directory.h \
	../filesys/filehdr.h\
	../filesys/filesys.h \
	../filesys/openfile.h\
	../filesys/pbitmap.h\
	../filesys/synchdisk.h

FILESYS_C =../filesys/directory.cc\
	../filesys/filehdr.cc\
	../filesys/filesys.cc\
	../filesys/pbitmap.cc\
	../filesys/openfile.cc\
	../filesys/synchdisk.cc\

FILESYS_O =directory.o filehdr.o filesys.o pbitmap.o openfile.o synchdisk.o

NETWORK_H = ../network/post.h

NETWORK_C = ../network/post.cc

NETWORK_O = post.o

##################################################################
#  You probably don't want to change anything below this point in
#  the file unless you are comfortable with GNU make and know what
#  you are doing...
##################################################################

THREAD_S = ../threads/switch.s

HFILES = $(LIB_H) $(MACHINE_H) $(THREAD_H) $(USERPROG_H) $(FILESYS_H) $(NETWORK_H)
CFILES = $(LIB_C) $(MACHINE_C) $(THREAD_C) $(USERPROG_C) $(FILESYS_C) $(NETWORK_C)

C_OFILES = $(LIB_O) $(MACHINE_O) $(THREAD_O) $(USERPROG_O) $(FILESYS_O) $(NETWORK_O)

S_OFILES = switch.o
OFILES = $(C_OFILES) $(S_OFILES)

$(PROGRAM): $(OFILES)
	$(LD) $(OFILES) $(LDFLAGS) -o $(PROGRAM)

$(C_OFILES): %.o:
	$(CC) $(CFLAGS) -c $<

switch.o: ../threads/switch.s
	$(CPP) $(CPP_AS_FLAGS) -P $(INCPATH) $(HOSTCFLAGS) ../threads/switch.s > swtch.s
	$(AS) -o switch.o swtch.s

depend: $(CFILES) $(HFILES)
	$(CC) $(INCPATH) $(DEFINES) $(HOSTCFLAGS) -DCHANGED -M $(CFILES) > makedep
	@echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@echo 'q' >>eddep
	ed - Makefile.dep < eddep
	rm eddep makedep 
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile.dep
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile.dep
	@echo '# see make depend above' >> Makefile.dep

clean:
	$(RM) -f $(OFILES)
	$(RM) -f swtch.s

distclean: clean
	$(RM) -f $(PROGRAM)
	$(RM) -f DISK_?
	$(RM) -f core
	$(RM) -f SOCKET_?

include Makefile.dep
