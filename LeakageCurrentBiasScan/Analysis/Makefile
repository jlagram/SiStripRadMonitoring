# Makefile for BDT codes

# $< = name of first dependancy
# $@ = name of target ("main")
# -c ask the compiler not to link
# -o specify target
#

#Define variables
CC=g++ #Compiler used
DEBUG = -g
CFLAGS = -I  $(DEBUG) #Compilation options
# LFLAGS = $(DEBUG) -L/home/nico/root/lib -lTMVA -lTMVAGui
# LFLAGS = -lTMVA #-lTMVAGui
ROOTFLAGS = `root-config --glibs --cflags`

SRCS = FitLeakageCurrent.C GetKink.C ComputeILeakCorrections.C #Source files are all files with .cc extension
HDRS = ../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.h  Code/Steps.h Code/Voltage.h Code/DCUCurrent.h Code/PSCurrent.h Code/FitFunctions.h #Header files are all files with .h extension
OBJS = $(SRCS:.cc=.o) #Object files are all files with .o extension, which have same names as source files

COMPUTEILEAKFIT = FitLeakageCurrent.exe

#.PHONY : $(wildcard *.o)  #Force to always recompile object


#Instructions
all: $(COMPUTEILEAKFIT)

#Obtain executables from object files

$(COMPUTEILEAKFIT):  FitLeakageCurrent.C ../../SignalBiasScan/Analysis/CommonTools/CurvesFunctions.h Code/Steps.h Code/Voltage.h Code/DCUCurrent.h Code/PSCurrent.h Code/FitFunctions.h
	@echo "-- Creating executable ./$(COMPUTEILEAKFIT) --"
	@$(CC) FitLeakageCurrent.C -o $@ $(ROOTFLAGS)
	@echo "-- Done --"
	

#Erase all objects and executable
clean:
	-rm -f *.o *.exe
	
