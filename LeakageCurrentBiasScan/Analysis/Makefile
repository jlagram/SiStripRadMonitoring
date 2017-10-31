# Makefile for BDT codes

# $< = name of first dependancy
# $@ = name of target ("main")
# -c ask the compiler not to link
# -o specify target
#

#Define variables
CC=g++ #Compiler used
DEBUG = -g
CFLAGS = -c  $(DEBUG) #Compilation options
# LFLAGS = $(DEBUG) -L/home/nico/root/lib -lTMVA -lTMVAGui
LFLAGS = -lTMVA #-lTMVAGui
ROOTFLAGS = `root-config --glibs --cflags`

SRCS = $(wildcard *.cc) #Source files are all files with .cc extension
HDRS = $(wildcard *.h) #Header files are all files with .h extension
OBJS = $(SRCS:.cc=.o) #Object files are all files with .o extension, which have same names as source files

COMPUTEILEAKCORR = ComputeILeakCorrections.exe

#.PHONY : $(wildcard *.o)  #Force to always recompile object


#Instructions
all: $(COMPUTEILEAKCORR)

#Obtain executables from object files

$(COMPUTEILEAKCORR): ComputeILeakCorrections.cc Code/Steps.h Code/Voltage.h Code/DCUCurrent.h Code/PSCurrent.h Code/FitFunctions.h
	@echo "-- Creating executable ./$(COMPUTEILEAKCORR) --"
	@$(CC) ComputeILeakCorrections.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"
	

#Erase all objects and executable
clean:
	-rm -f *.o *.exe
	
