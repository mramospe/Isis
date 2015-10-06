#===========================================#
# Main compiling file for the Isis project  #
#-------------------------------------------#
# AUTHOR: Miguel Ramos Pernas		    #
# e-mail: miguel.ramos.pernas@cern.ch	    #
#===========================================#

# ------------------------------------------
# Compiler and shell options
COMPILER  = g++
CFLAGS    = `Isis-config --cflags` -shared -fPIC
SHELL     = /bin/bash

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
	@echo " Installation finished"

# ------------------------------------------
# Root flags
ROOT_LIBS = `Isis-config --rlibs`

# ------------------------------------------
# Include paths
INCLUDE = `Isis-config --incdir`

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
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) $(ROOT_LIBS) -o $@

$(ANALYSIS_LIB): $(ANALYSIS_OBJECTS)
	@echo " Creating shared library for Analysis package"
	$(COMPILER) $(CFLAGS) -o $@ $(ANALYSIS_SOURCE)/*.o

# ------------------------------------------
# Compiles the Tools
Tools/%.out: Tools/%.cpp
	$(COMPILER) `Isis-config --cflags` $< $(INCLUDE) $(ROOT_LIBS) -o $@

# ---------------------------------------------------------
# Function to remove all the installation files and folders
clean:
	@rm -r lib
	@echo " Removed library folder at: $ISIS/lib"
	@rm General/source/*.o
	@echo " Removed object files at: $ISIS/General/source"
	@rm Analysis/source/*.o
	@echo " Removed object fiels at: $ISIS/Analysis/source"
	@rm Tools/*.out
	@echo " Removed compiled files at: $ISIS/Tools"
