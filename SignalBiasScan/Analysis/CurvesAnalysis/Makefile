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

FIT = Fit.exe
FITALL = FitAll.exe
COMPARECURVES = CompareCurves.exe
DRAWKINKVSLUMI = DrawKinkVsLumi.exe
TRACKERMAP = Tracker_Vfd_Map.exe

CURVFUNC = ../CommonTools/CurvesFunctions.h
MODFLU = ../CommonTools/ModFluence.h
#.PHONY : $(wildcard *.o)  #Force to always recompile object


#Instructions
all: $(FIT) $(FITALL) $(COMPARECURVES) $(DRAWKINKVSLUMI) $(TRACKERMAP)

#Obtain executables from object files

$(FIT): Fit.cc Fit.h $(CURVFUNC)
	@echo "-- Creating executable ./$(FIT) --"
	@$(CC) Fit.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"
	
$(FITALL): FitAll.cc Fit.h $(CURVFUNC)
	@echo "-- Creating executable ./$(FITALL) --"
	@$(CC) FitAll.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"
	
$(COMPARECURVES): CompareCurves.cc $(CURVFUNC)
	@echo "-- Creating executable ./$(COMPARECURVES) --"
	@$(CC) CompareCurves.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"
	
$(DRAWKINKVSLUMI): DrawKinkVsLumi.cc $(CURVFUNC) $(MODFLU)
	@echo "-- Creating executable ./$(DRAWKINKVSLUMI) --"
	@$(CC) DrawKinkVsLumi.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"
	
$(TRACKERMAP): Tracker_Vfd_Map.cc Fit.h
	@echo "-- Creating executable ./$(TRACKERMAP) --"
	@$(CC) Tracker_Vfd_Map.cc -o $@ $(ROOTFLAGS) $(LFLAGS)
	@echo "-- Done --"


#Erase all objects and executable
clean:
	@-rm -f *.o *.exe
