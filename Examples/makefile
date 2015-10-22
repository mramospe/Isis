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

MakeBDT: MakeBDT.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

CLsScan: CLsScan.cpp
	$(COMPILER) $(CFLAGS) $^ $(MAIN) $(DIRSLIBS) -o $@.out

