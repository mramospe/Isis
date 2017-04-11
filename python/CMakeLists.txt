#----------------------------------------------------------------------------
# Defines the way to manage the python source. It will be translated to
# byte-code using the script compile.py.
project(Isis-python)
set(PYSOURCE ${PROJECT_SOURCE_DIR})
set(PYBINARY ${INSTALL_PYTHON_DIR}/Isis)

#----------------------------------------------------------------------------
# Defines the dependencies on the boost implementations to python
set(IBOOST_TARGETS "")
add_subdirectory(iboost)

#----------------------------------------------------------------------------
# Target for the python source
if( NOT ${PYSOURCE}/Isis STREQUAL ${PYBINARY} )
  copy_directory(${PYSOURCE}/Isis ${PYBINARY})
endif()
add_custom_target(Isis-python ALL python ${PYSOURCE}/compile.py ${PYSOURCE}/Isis ${PYBINARY} DEPENDS ${IBOOST_TARGETS})
