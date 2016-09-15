all: src/MyDict.cxx
	g++ `root-config --cflags` -o genTree src/LHEWriter.cc src/genTree.cc $^ `root-config --glibs`
