#----------------------------------------------------------------------------
# Find the Python NumPy package. The following variables will be set:
# PYTHON_NUMPY_INCLUDE_DIR
# PYTHON_NUMPY_FOUND

#----------------------------------------------------------------------------
# Access the python interpreter
if(NOT PYTHON_EXECUTABLE)
  if(NumPy_FIND_QUIETLY)
    find_package(PythonInterp QUIET)
  else()
    find_package(PythonInterp)
    set(__numpy_out 1)
  endif()
endif()

#----------------------------------------------------------------------------
# Find the include path and the version
if (PYTHON_EXECUTABLE)

  set(__numpy_import "from __future__ import print_function")
  
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
    "${__numpy_import}\ntry: import numpy; print(numpy.get_include(), end='')\nexcept: pass\n"
    OUTPUT_VARIABLE __numpy_path
    )
	  
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
    "${__numpy_import}\ntry: import numpy; print(numpy.__version__, end='')\nexcept: pass\n"
    OUTPUT_VARIABLE __numpy_version
    )

  unset(__numpy_import)
  
elseif(__numpy_out)
  message(FATAL_ERROR "Python executable not found")
endif(PYTHON_EXECUTABLE)

#----------------------------------------------------------------------------
# Path to the headers
find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h
  HINTS "${__numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)

if(PYTHON_NUMPY_INCLUDE_DIR)
  set(PYTHON_NUMPY_FOUND 1 CACHE INTERNAL "NumPy not found")
endif(PYTHON_NUMPY_INCLUDE_DIR)

#----------------------------------------------------------------------------
# Handle the package features
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NumPy REQUIRED_VARS PYTHON_NUMPY_INCLUDE_DIR
  VERSION_VAR __numpy_version
  )
