# Compile macros using ROOT
UNAMEP := $(shell, uname -p)
CC=g++
ifeq ($(UNAMEP),$(x86_64))
	AFLAG=-m64
else
	AFLAG=-m32
endif 

WFLAGS=-Wall -Wextra
CFLAGS=$(AFLAG) -pg -ggdb -I $(ROOTSYS)/include -I $(PWD)/include -O0 $(WFLAGS)
LDFLAGS=$(AFLAG) -pg -L$(LD_LIBRARY_PATH) -L$(ROOTSYS)/lib  -lCint -lRIO -lTree -lNet -lHist -lGpad -lGraf -lGraf3d  -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lGui  -lm -ldl -rdynamic -lFoam -lCore
ROOFITLDFLAGS=-lRooFit -lRooFitCore -lMinuit -lRooStats -lMathMore

all: JetColor

JetColor: JetColor.o JetColor_dict.o
	$(CC) JetColor.o JetColor_dict.o -o JetColor $(LDFLAGS) 
JetColor.o: ./bin/JetColor.C ./JetColor_LinkDef.h
	$(CC) $(CFLAGS) -c ./bin/JetColor.C
	rootcint -v4 -f JetColor_dict.cxx -c -p JetColor_LinkDef.h
JetColor_dict.o: ./JetColor_dict.cxx
	$(CC) $(CFLAGS) -fPIC -c ./JetColor_dict.cxx
clean:
	rm -rf *o  JetColor
