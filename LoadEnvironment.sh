# Setup message
echo " *** Isis project ***"

# Exports the different folders of the project
export ISIS=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
export ISIS_ANALYSIS=$ISIS/Analysis
export ISIS_GENERAL=$ISIS/General
export ISIS_LIB=$ISIS/lib
export ISIS_MAKEFILES=$ISIS/Makefiles
export ISIS_PYTHON=$ISIS/python
export ISIS_TOOLS=$ISIS/Tools

# If the library directory does not exist it is created
if [ ! -d "$ISIS/lib" ]; then
    mkdir $ISIS/lib
fi

# Exports the python modules
export PYTHONPATH=$PYTHONPATH:$ISIS_PYTHON

# The library path is updated with the Isis libraries
export LD_LIBRARY_PATH=$ISIS_LIB:$LD_LIBRARY_PATH

# Compiles the c++ code
make -C $ISIS -f makefile

# Defines the alias to the tools
alias BDTBrowser='$ISIS_TOOLS/BDTBrowser.C'
alias ChangeWords='python $ISIS_TOOLS/ChangeWords.py'
alias GetLuminosity='$ISIS_TOOLS/./GetLuminosity.out'
alias MergeFilesByEvents='$ISIS_TOOLS/./MergeFilesByEvents.out'
alias ReorderTreeByEvents='$ISIS_TOOLS/./ReorderTreeByEvents.out'
