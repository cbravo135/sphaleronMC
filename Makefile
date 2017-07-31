CXXFLAGS=$(shell root-config --cflags)
GLIBS=$(shell root-config --glibs)

SRCS:=$(wildcard src/*.cc)

all: genTree

MyDict.cxx: include/LinkDef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $^

lib/libMyLib.so: MyDict.cxx 
	g++ -shared -o$@ `root-config --ldflags` -fPIC $(CXXFLAGS) -I$(ROOTSYS)/include $^

genTree: MyDict.cxx $(SRCS)
	g++ $(CXXFLAGS) $(SRCS) -o $@ -I$(HOME)/local/include/ $(GLIBS) `lhapdf-config --cflags --ldflags`
