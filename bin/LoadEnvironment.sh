#============================================#
# Loads the environment for the Isis project #
#--------------------------------------------#
# AUTHOR: Miguel Ramos Pernas		     #
# e-mail: miguel.ramos.pernas@cern.ch	     #
#============================================#

# Setup message
echo " *** Isis project ***"

# Gets the path to this file
currpath=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

# Exports the different folders of the project
export ISIS=${currpath:0:(${#currpath}-4)}
export ISIS_MAKEFILES=$ISIS/Makefiles
export ISIS_PYTHON=$ISIS/python
export ISIS_TOOLS=$ISIS/Tools

# Displays the path to the Isis folder
echo " Loading source from: $ISIS"

# Extends the path to bash scripts with this one
PATH=$PATH:$currpath
echo " Extended shell path to $currpath"

# The library path is updated with the Isis libraries
export LD_LIBRARY_PATH=$ISIS/lib:$LD_LIBRARY_PATH
echo " Exported c++ libraries from: $ISIS/lib"

# Exports the python modules
export PYTHONPATH=$PYTHONPATH:$ISIS_PYTHON
echo " Extended python path from: $ISIS_PYTHON"

# Defines the alias to the tools
alias BDTBrowser='root -l $ISIS_TOOLS/BDTBrowser.C'
alias ChangeWords='python $ISIS_TOOLS/ChangeWords.py'
alias GetLuminosity='$ISIS_TOOLS/./GetLuminosity.out'
alias MergeFilesByEvents='$ISIS_TOOLS/./MergeFilesByEvents.out'
alias ReorderTreeByEvents='$ISIS_TOOLS/./ReorderTreeByEvents.out'
alias SeeDifferences='python $ISIS_TOOLS/SeeDifferences.py'
echo " Defined alias for tools at: $ISIS_TOOLS"