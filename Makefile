all: MyDict.cxx
	g++ `root-config --cflags` -o genTree genTree.C $^ `root-config --glibs`
