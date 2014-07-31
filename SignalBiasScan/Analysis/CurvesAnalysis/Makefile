ObjSuf        = o
SrcSuf        = C
LogSuf        = log

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)

DEFINES       =

CXX           = g++
CXXFLAGS      = -O -Wall -fPIC $(DEFINES) -Wno-unused-result -Wshadow
CXXFLAGS      += $(ROOTCFLAGS) -I./

LD            = g++
LDFLAGS       = -g -O -Wall -fPIC -Wl,--no-undefined
LDFLAGS       += $(ROOTLIBS)  -lEG -lTMVA

SOFLAGS       = -shared
LIBS          =

# Uncomment this line if you want to use a script to parse & colorize gcc output
# (You can also export this variable from your bashrc)
#GCCPARSER     = 2>&1 | python ~/littleScripts/colorGcc.py

#------------------------------------------------------------------------------
SOURCES       = $(wildcard *.$(SrcSuf))
OBJECTS       = $(SOURCES:.$(SrcSuf)=.$(ObjSuf))
EXECUTABLES   = $(SOURCES:.$(SrcSuf)=)
LOGS          = $(SOURCES:.$(SrcSuf)=.$(LogSuf))
#------------------------------------------------------------------------------
DEFAULT_TARGET = DrawKinkVsLumi
#DEFAULT_TARGET = FitAll
#------------------------------------------------------------------------------

all: compile run

compile: $(DEFAULT_TARGET)

run:
	./$(DEFAULT_TARGET) | tee $(DEFAULT_TARGET).$(LogSuf)

#browse:
#       export PLOTS_TO_BROWSE=`ls ../plots/$(shell basename `pwd`)/*.root`; root -l .browse

clean:
	@echo "Cleaning..."
	@echo "rm -f" $(OBJECTS) $(EXECUTABLES) $(LOGS)

#------------------------------------------------------------------------------

$(DEFAULT_TARGET): $(DEFAULT_TARGET).$(SrcSuf)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LIBS) $(GCCPARSER)
