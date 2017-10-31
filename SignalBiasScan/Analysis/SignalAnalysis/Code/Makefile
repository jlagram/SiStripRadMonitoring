GCC = g++
LD = g++
CXXFLAGS = $(shell root-config --cflags) 
LIBS = $(shell root-config --libs)

SRCS = ../../VoltageSteps/Code/VoltageStepsMaker.C ../../Clustering/Code/ClustersMaker.C SignalAnalysisTreeMaker.C Run_TreeMaker_compiled.C #Dict.C
OBJS = ../../VoltageSteps/Code/VoltageStepsMaker.o ../../Clustering/Code/ClustersMaker.o SignalAnalysisTreeMaker.o Run_TreeMaker_compiled.o #Dict.o

PROGRAM = Run_TreeMaker_compiled
HEADERS = ../../../interface/TreeEvent.h
DEFINES = -DNO_ORCA_CLASSES



all:	Dict compile_dict compile link


link:	$(OBJS)
	@echo "Linking object files ..."
	$(LD) $(OBJS) $(LIBS) -o $(PROGRAM)


compile:	$(OBJS)
#Implicit Makefile rule : creates objects using corresponding .C files & CXXFLAGS !	

compile_dict: 	Dict.C
	$(LD) -c Dict.C $(CXXFLAGS) -o Dict.o
#Need to compile Dict.C separately (Makefile can't find rule ?)


Dict: $(HEADERS) LinkDef.h
	@echo "Generating dictionary Dict.C ..."
	rootcint -f Dict.C -c $(DEFINES) $(HEADERS) LinkDef.h
	@echo "Compiling source files ..."


clean:; @rm -f $(OBJS) Dict.h Dict.o Dict.C $(PROGRAM) *~ */*~

