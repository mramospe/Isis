#============================================#
# Loads the environment for the Isis project #
#--------------------------------------------#
# AUTHOR: Miguel Ramos Pernas		     #
# e-mail: miguel.ramos.pernas@cern.ch	     #
#============================================#

# Setup message
echo "*** Isis project ***"

# Gets the path to this file
currpath=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

# Exports the different folders of the project
export ISIS=${currpath:0:(${#currpath}-4)}
export ISIS_MAKEFILES=$ISIS/Makefiles
export ISIS_PYTHON=$ISIS/python
export ISIS_TOOLS=$ISIS/Tools

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

# Defines the alias to the tools
alias BDTBrowser='root -l $ISIS_TOOLS/BDTBrowser.C'
alias changeWords='python $ISIS_TOOLS/changeWords.py'
alias convertVars='$ISIS_TOOLS/./convertVars.out'
alias getLuminosity='$ISIS_TOOLS/./getLuminosity.out'
alias mergeFilesByEvents='$ISIS_TOOLS/./mergeFilesByEvents.out'
alias readCuts='$ISIS_TOOLS/./readCuts.out'
alias reorderTreeByEvents='$ISIS_TOOLS/./reorderTreeByEvents.out'
echo "Defined alias for tools at: $ISIS_TOOLS"