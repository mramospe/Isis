#============================================
# Loads the environment for the Isis project
#--------------------------------------------
# AUTHOR: Miguel Ramos Pernas
# e-mail: miguel.ramos.pernas@cern.ch
#============================================

# Setup message
echo "*** Isis project (v@PROJECT_VERSION@)***"

# Exports the different folders of the project
export ISIS_ROOT="@CMAKE_BINARY_DIR@"
export ISIS_BIN="@INSTALL_BIN_DIR@"
export ISIS_LIB="@INSTALL_LIB_DIR@"
export ISIS_PYTHON="@INSTALL_PYTHON_DIR@"

# Displays the path to the Isis folder
echo "Loading source from: $ISIS_ROOT"

# Function to extend a given path if it has not been done yet
function extend {

    if [[ ${!1} != *${!2}* ]]; then
	export $1=${!2}:${!1}
    fi
}

# Export cmake variables
extend CMAKE_PREFIX_PATH ISIS_ROOT
echo "Extended CMake prefix path with: $ISIS_ROOT"

# Extend the path for bash scripts
extend PATH ISIS_BIN
echo "Extended shell path with: $ISIS_BIN"

# The library path is updated with the Isis libraries
extend LD_LIBRARY_PATH ISIS_LIB
echo "Exported c++ libraries with: $ISIS_LIB"

# Export the path to the python modules
extend PYTHONPATH ISIS_PYTHON
echo "Extended python path with: $ISIS_PYTHON"
