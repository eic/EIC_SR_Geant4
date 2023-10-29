ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
EXTRALIBS += $(shell $(ROOTSYS)/bin/root-config --libs)
LDLIBS += $(shell $(ROOTSYS)/bin/root-config --libs)

CXX           = g++
CXXFLAGS      = -g -Wall -fPIC -Wno-deprecated

NGLIBS         = -Wl, $(ROOTGLIBS) 
NGLIBS        += -lMinuit

CXXFLAGS      += $(ROOTCFLAGS)
CXX           += -I./
LIBS           = $(ROOTLIBS) 

GLIBS          = $(filter-out -lNew, $(NGLIBS))

CXX	      += -I./obj/
OUTLIB	      = ./obj/
.SUFFIXES: .cc,.C
.PREFIXES: ./obj/

#----------------------------------------------------#

all: exe

exe: ana.cc obj/AnalysisSR.o
	$(CXX) $(CXXFLAGS) -o exe $(OUTLIB)/*.o $(GLIBS) $<

obj/AnalysisSR.o: src/AnalysisSR.cc src/AnalysisSR.hh 
	$(CXX) $(CXXFLAGS) -c -I. -o $(OUTLIB)AnalysisSR.o $<

clean:
	rm -f *~
	rm -f *.png
	rm -f exe
	rm -f $(OUTLIB)*.o
	rm -rf *.dSYM
