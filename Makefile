CXXFLAGS=$(shell root-config --cflags)
GLIBS=$(shell root-config --glibs)

SRCS:=$(wildcard src/*.cc)

all: genTree

MyDict.cxx: include/LinkDef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $^
	 
libMyLib.so: MyDict.cxx 
	g++ -shared -o$@ `root-config --ldflags` $(CXXFLAGS) -I$(ROOTSYS)/include $^

genTree: MyDict.cxx $(SRCS)
	g++ $(CXXFLAGS) $(GLIBS) $(SRCS) -o $@
