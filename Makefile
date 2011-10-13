# Compile macros using ROOT
CC=g++

export PATH:=$(ROOTSYS)/bin:$(PATH)
#export LD_LIBRARY_PATH:=$(ROOTSYS)/lib:$(LD_LIBRARY_PATH)
export LD_LIBRARY_PATH=/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/16.6.5/DetCommon/16.6.5/InstallArea/i686-slc5-gcc43-opt/bin/../../../../../sw/lcg/app/releases/ROOT/5.26.00e_python2.6/i686-slc5-gcc43-opt/root/lib



INCDIR=$(PWD)/include:/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/16.6.5/DetCommon/16.6.5/InstallArea/i686-slc5-gcc43-opt/bin/../../../../../sw/lcg/app/releases/ROOT/5.26.00e_python2.6/i686-slc5-gcc43-opt/root/include


WFLAGS=-Wall -Wextra
CFLAGS=-m32 -pg -ggdb -I$(INCDIR) -O0 $(WFLAGS)
LDFLAGS=-m32 -pg -L$(LD_LIBRARY_PATH)  -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lGui  -lm -ldl -rdynamic -lFoam #-lCore
ROOFITLDFLAGS=-lRooFit -lRooFitCore -lMinuit -lRooStats #-lMathMore

all: JetColor

JetColor: JetColor.o JetColor_dict.o
	$(CC) JetColor.o JetColor_dict.o -o JetColor $(LDFLAGS) 
#SimpleCounting: SimpleCounting.o style.o printing.o
#	$(CC) SimpleCounting.o style.o printing.o -o SimpleCounting $(LDFLAGS)

JetColor.o: ./bin/JetColor.C ./JetColor_LinkDef.h
	$(CC) $(CFLAGS) -c ./bin/JetColor.C
	rootcint -v4 -f JetColor_dict.cxx -c -p JetColor_LinkDef.h
JetColor_dict.o: ./JetColor_dict.cxx
	$(CC) $(CFLAGS) -fPIC -c ./JetColor_dict.cxx
clean:
	rm -rf *o  JetColor
