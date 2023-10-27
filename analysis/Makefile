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

all: ana

ana: ana.cc obj/AnalysisSR
	$(CXX) $(CXXFLAGS) -o ana $(OUTLIB)/*.o $(GLIBS) $<

obj/AnalysisSR.o: src/AnalysisSR.C src/AnalysisSR.h 
	$(CXX) $(CXXFLAGS) -c -I. -o $(OUTLIB)AnalysisSR.o $<

clean:
	rm -f *~
	rm -f ana
	rm -f $(OUTLIB)*.o
	rm -rf *.dSYM
