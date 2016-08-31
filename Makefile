all: src/MyDict.cxx
	g++ `root-config --cflags` -o genTree src/genTree.cc $^ `root-config --glibs`
