#===========================================#
# Makefile for Isis project examples	    #
#-------------------------------------------#
# AUTHOR: Miguel Ramos Pernas               #
# e-mail: miguel.ramos.pernas@cern.ch       #
#===========================================#

# Compiler
COMPILER = g++

# Path to Isis include folders and libraries
CFLAGS   = `Isis-config --cflags`
DIRSLIBS = `Isis-config --incdirs --libs`

help:
	@echo " Usage: make < example name >"

CLsScan: CLsScan.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

MakeBDT: MakeBDT.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

ProgBar: ProgBar.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

ToyMC: ToyMC.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

AdapBin: AdapBin.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

ParseStr: ParseStr.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out