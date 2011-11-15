# Compile macros using ROOT
CC=g++

WFLAGS=-Wall -Wextra
CFLAGS=-m64 -pg -ggdb -I $(ROOTSYS)/include -I $(PWD)/include -O0 $(WFLAGS)
LDFLAGS=-m64 -pg -L$(LD_LIBRARY_PATH) -L$(ROOTSYS)/lib  -lCint -lRIO -lTree -lNet -lHist -lGpad -lGraf -lGraf3d  -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lGui  -lm -ldl -rdynamic -lFoam -lCore
ROOFITLDFLAGS=-lRooFit -lRooFitCore -lMinuit -lRooStats -lMathMore

all: JetColor

JetColor: JetColor.o JetColor_dict.o
	$(CC) JetColor.o JetColor_dict.o -o JetColor $(LDFLAGS) 
JetColor.o: ./bin/JetColor.C ./JetColor_LinkDef.h
	$(CC) $(CFLAGS) -c ./bin/JetColor.C
	rootcint -v4 -f JetColor_dict.cxx -c -p JetColor_LinkDef.h
	echo "Done Building JetColor.o"
JetColor_dict.o: ./JetColor_dict.cxx
	$(CC) $(CFLAGS) -fPIC -c ./JetColor_dict.cxx
clean:
	rm -rf *o  JetColor
