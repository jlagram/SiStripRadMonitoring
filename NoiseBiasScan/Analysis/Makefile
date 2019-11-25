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
LFLAGS = $(DEBUG) -lTMVA #-lTMVAGui
ROOTFLAGS = `root-config --glibs --cflags`

SRCS = $(wildcard *.cc) #Source files are all files with .cc extension
HDRS = $(wildcard *.h) #Header files are all files with .h extension
OBJS = $(SRCS:.cc=.o) #Object files are all files with .o extension, which have same names as source files

READ_NOISE = readNoise.exe #Name of executable file
EXTRACT_NOISE = Extract_Noise_Per_DetID.exe
NOISE_PLOT = Produce_Noise_vs_Bias_Plots.exe
#STUDIES = Studies_Fit_Model.exe
MERGE_APVS = Merge_APVs_Results.exe

# .PHONY : $(READ_NOISE)  #Force to always recompile object




#Instructions
all: $(EXTRACT_NOISE) $(NOISE_PLOT) $(MERGE_APVS) #$(STUDIES)

#Obtain executables from object files
# $(READ_NOISE) : readNoise.cxx
# 	@echo "-- Creating executable ./$(READ_NOISE) --"
# 	@$(CC) readNoise.cxx -o $@ $(ROOTFLAGS) $(LFLAGS)
# 	@echo "-- Done --"

$(EXTRACT_NOISE) : Extract_Noise_Per_DetID.cxx Helper_Functions.h
	@echo "-- Creating executable ./$(EXTRACT_NOISE) --"
	@$(CC) Extract_Noise_Per_DetID.cxx -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"

$(NOISE_PLOT) : Produce_Noise_vs_Bias_Plots.cxx Helper_Functions.h
	@echo "-- Creating executable ./$(NOISE_PLOT) --"
	@$(CC) Produce_Noise_vs_Bias_Plots.cxx -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"

$(STUDIES) : Produce_Noise_vs_Bias_Plots.cxx Helper_Functions.h
	@echo "-- Creating executable ./$(STUDIES) --"
	@$(CC) Studies_Fit_Model.cxx -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"

$(MERGE_APVS) : Merge_APVs_Results.cxx Helper_Functions.h
	@echo "-- Creating executable ./$(MERGE_APVS) --"
	@$(CC) Merge_APVs_Results.cxx -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"


#Obtain objects from source and header files
%.o: %.cc
	@echo "-- Compiling --"
	@$(CC) $(ROOTFLAGS) $(CFLAGS) $< -o $@



#Erase all objects and executable
clean:
	@-rm -f *.o *.exe
