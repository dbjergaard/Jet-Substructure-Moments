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

JetColor: ./obj/JetColor.o ./obj/JetColor_dict.o ./obj/JetMomentCalculator.o
	$(CC) -o JetColor ./obj/JetColor.o ./obj/JetMomentCalculator.o ./obj/JetColor_dict.o $(LDFLAGS) 
obj/JetColor.o: ./bin/JetColor.C ./include/JetColor_LinkDef.h #./obj/JetMomentCalculator.o
	$(CC) $(CFLAGS) -c ./bin/JetColor.C  -o ./obj/JetColor.o #./obj/JetMomentCalculator.o
	rootcint -v4 -f ./src/JetColor_dict.cxx -c -p ./include/JetColor_LinkDef.h
obj/JetColor_dict.o: ./src/JetColor_dict.cxx
	$(CC) $(CFLAGS) -fPIC -c ./src/JetColor_dict.cxx -o ./obj/JetColor_dict.o
obj/JetMomentCalculator.o: ./src/JetMomentCalculator.C ./include/JetMomentCalculator.h
	$(CC) $(CFLAGS) -c ./src/JetMomentCalculator.C -o ./obj/JetMomentCalculator.o
clean:
	rm -f ./obj/*.o  gmon.out JetColor
