#============================================
# Loads the environment for the Isis project
#--------------------------------------------
# AUTHOR: Miguel Ramos Pernas
# e-mail: miguel.ramos.pernas@cern.ch
#============================================

# Setup message
echo "*** Isis project ***"

# Gets the path to this file
currpath=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

# Exports the different folders of the project
export ISIS=${currpath:0:(${#currpath}-4)}
export ISIS_BIN=$currpath
export ISIS_PYTHON=$ISIS/python
export ISIS_SCRIPTS=$ISIS/scripts

# Displays the path to the Isis folder
echo "Loading source from: $ISIS"

# Extends the path to bash scripts with this one
if [[ $PATH != *$currpath* ]]; then
    export PATH=$currpath:$PATH
fi
echo "Extended shell path from: $currpath"

# The library path is updated with the Isis libraries
if [[ $LD_LIBRARY_PATH != *$ISIS/lib* ]]; then
    export LD_LIBRARY_PATH=$ISIS/lib:$LD_LIBRARY_PATH
fi
echo "Exported c++ libraries from: $ISIS/lib"

# Exports the python modules
if [[ $PYTHONPATH != *$ISIS_PYTHON* ]]; then
    export PYTHONPATH=$ISIS_PYTHON:$PYTHONPATH
fi
echo "Extended python path from: $ISIS_PYTHON"

# Export cmake variables
export CMAKE_PREFIX_PATH=$ISIS:$CMAKE_PREFIX_PATH
echo "Extended CMake prefix path with: $ISIS"

# Defines the alias to the scripts
alias readCuts='python $ISIS_SCRIPTS/readCuts.py'
echo "Defined alias for scripts at: $ISIS_SCRIPTS"
