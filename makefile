#===========================================
# Main compiling file for the Isis project
#===========================================

# ------------------------------------------
# Compiler options

COMPILER = g++
CFLAGS   = -Wall -shared -fPIC

# ------------------------------------------
# Root flags

ROOT_FLAGS = `root-config --cflags --libs` -lEG -lHtml -lRooFitCore -lMinuit -lRooFit -lTMVA

# ------------------------------------------
# Include paths definition

PACKAGES = General Analysis
INCLUDE  = $(foreach dir, $(PACKAGES), -I$(dir)/include)

# ------------------------------------------
# Source path and objects definition

GENERAL_SOURCE   = General/source
GENERAL_OBJECTS  = $(patsubst %.cpp, %.o, $(wildcard $(GENERAL_SOURCE)/*.cpp))
GENERAL_LIB      = lib/libGeneral.so
ANALYSIS_SOURCE  = Analysis/source
ANALYSIS_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard $(ANALYSIS_SOURCE)/*.cpp))
ANALYSIS_LIB     = lib/libAnalysis.so
TOOLS_EXECS      = $(patsubst %.cpp, %.out, $(wildcard Tools/*.cpp))

# ------------------------------------------
# Main compiling function

all: $(GENERAL_LIB) $(ANALYSIS_LIB) $(TOOLS_EXECS)
	@echo " Files are up to date"

# ------------------------------------------
# Compiles the General package

$(GENERAL_SOURCE)/%.o: $(GENERAL_SOURCE)/%.cpp
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) -o $@

$(GENERAL_LIB): $(GENERAL_OBJECTS)
	@echo " Creating shared library for General package"
	$(COMPILER) $(CFLAGS) -o $@ $(GENERAL_SOURCE)/*.o

# ------------------------------------------
# Compiles the Analysis package

$(ANALYSIS_SOURCE)/%.o: $(ANALYSIS_SOURCE)/%.cpp
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) $(ROOT_FLAGS) -o $@

$(ANALYSIS_LIB): $(ANALYSIS_OBJECTS)
	@echo " Creating shared library for Analysis package"
	$(COMPILER) $(CFLAGS) -o $@ $(ANALYSIS_SOURCE)/*.o

# ------------------------------------------
# Compiles the Tools

Tools/%.out: Tools/%.cpp
	$(COMPILER) $< $(ROOT_FLAGS) -o $@
