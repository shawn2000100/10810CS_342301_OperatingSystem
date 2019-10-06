PREFIX=decstation-ultrix- # add crosscompiler prefix here i.e. decstation-ultrix-
INCLUDEDIR=               # add path to include directories for crosscompiler environment here: 
                          # don't forget the -I tag before the directory
                          # i.e: -I/usr/lcoal/nachosxdev/include -I/usr/local/nachosxdev/include/g++-3


all: stack inheritstack templatestack

stack: stack.h stack.cc
	$(PREFIX)g++ $(INCLUDEDIR) -o stack stack.cc

inheritstack: inheritstack.h inheritstack.cc list.h list.cc
	$(PREFIX)g++ $(INCLUDEDIR) -o inheritstack inheritstack.cc list.cc

templatestack: templatestack.h templatestack.cc
	$(PREFIX)g++ $(INCLUDEDIR) -o templatestack templatestack.cc
