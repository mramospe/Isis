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
GENERAL_SOURCE     = General/source
GENERAL_OBJECTS    = $(patsubst %.cpp, %.o, $(wildcard $(GENERAL_SOURCE)/*.cpp))
GENERAL_LIB        = lib/libGeneral.so
ANALYSIS_SOURCE    = Analysis/source
ANALYSIS_OBJECTS   = $(patsubst %.cpp, %.o, $(wildcard $(ANALYSIS_SOURCE)/*.cpp))
ANALYSIS_LIB       = lib/libAnalysis.so
RFANALYSIS_SOURCE  = Analysis/source/RooFit
RFANALYSIS_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard $(RFANALYSIS_SOURCE)/*.cpp))
RFANALYSIS_LIB     = lib/libRFAnalysis.so
TOOLS_EXECS        = $(patsubst %.cpp, %.out, $(wildcard Tools/*.cpp))

# ------------------------------------------
# Main compiling function
all: $(GENERAL_LIB) $(ANALYSIS_LIB) $(RFANALYSIS_LIB) $(TOOLS_EXECS)
	@echo "Installation finished"

# ------------------------------------------
# Root flags
ROOT_LIBS = `Isis-config --rlibs`

# ------------------------------------------
# Include paths
INCLUDE = `Isis-config --incdirs`

# ------------------------------------------
# Compiles the General package
$(GENERAL_SOURCE)/%.o: $(GENERAL_SOURCE)/%.cpp
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) -o $@

$(GENERAL_LIB): $(GENERAL_OBJECTS)
	$(COMPILER) $(CFLAGS) -o $@ $(GENERAL_SOURCE)/*.o
	@echo "Created shared library for General package"

# ------------------------------------------
# Compiles the Analysis package
$(ANALYSIS_SOURCE)/%.o: $(ANALYSIS_SOURCE)/%.cpp
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) $(ROOT_LIBS) -o $@

$(ANALYSIS_LIB): $(ANALYSIS_OBJECTS)
	$(COMPILER) $(CFLAGS) -o $@ $(ANALYSIS_SOURCE)/*.o
	@echo "Created shared library for Analysis package"

# ----------------------------------------------
# Compiles the RooFit-dependent Analysis package
$(RFANALYSIS_SOURCE)/%.o: $(RFANALYSIS_SOURCE)/%.cpp
	$(COMPILER) $(CFLAGS) -c $^ $(INCLUDE) $(ROOT_LIBS) -o $@

$(RFANALYSIS_LIB): $(RFANALYSIS_OBJECTS)
	$(COMPILER) $(CFLAGS) -o $@ $(RFANALYSIS_SOURCE)/*.o
	@echo "Created shared library for RooFit-Analysis package"

# ------------------------------------------
# Compiles the Tools
Tools/%.out: Tools/%.cpp
	$(COMPILER) `Isis-config --cflags --incdirs` $< `Isis-config --libs` -o $@

# ---------------------------------------------------------
# Function to remove all the installation files and folders
clean:
	@if [ -a lib ]; \
	then \
		rm -r lib; \
		echo " Removed library folder: lib"; \
	fi;
	@if [ "$(wildcard $(GENERAL_SOURCE)/*.o)" != "" ]; \
	then \
		rm $(GENERAL_SOURCE)/*.o; \
		echo " Removed compiled files at: $(GENERAL_SOURCE)"; \
	fi;
	@if [ "$(wildcard $(ANALYSIS_SOURCE)/*.o)" != "" ]; \
	then \
		rm $(ANALYSIS_SOURCE)/*.o; \
		echo " Removed compiled files at: $(ANALYSIS_SOURCE)"; \
	fi;
	@if [ "$(wildcard $(RFANALYSIS_SOURCE)/*.o)" != "" ]; \
	then \
		rm $(RFANALYSIS_SOURCE)/*.o; \
		echo " Removed compiled files at: $(RFANALYSIS_SOURCE)"; \
	fi;
	@if [ "$(wildcard Tools/*.out)" != "" ]; \
	then \
		rm Tools/*.out; \
		echo " Removed compiled files at: Tools"; \
	fi;
	@if [ "$(wildcard Examples/*.out)" != "" ]; \
	then \
		rm Examples/*.out; \
		echo " Removed compiled files at: Examples"; \
	fi;
	@if [ "$(wildcard Examples/files/*.root)" != "" ]; \
	then \
		rm Examples/files/*.root; \
		echo " Removed root files at: Examples/files"; \
	fi;

