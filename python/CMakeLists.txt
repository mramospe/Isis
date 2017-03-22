#----------------------------------------------------------------------------
# Defines the way to manage the python source. It will be translated to
# byte-code using the script compile.py.
project(Isis-python)
set(PYSOURCE ${PROJECT_SOURCE_DIR})
set(PYBINARY ${INSTALL_PYTHON_DIR}/Isis)

#----------------------------------------------------------------------------
# Create python binary directory
file(MAKE_DIRECTORY ${PYBINARY})

#----------------------------------------------------------------------------
# Defines the dependencies on the boost implementations to python
set(BOOST_TARGETS "")
add_subdirectory(IBoost)

#----------------------------------------------------------------------------
# Target for the python source
add_custom_target(Isis-python ALL python ${PYSOURCE}/compile.py ${PYSOURCE}/Isis ${PYBINARY} DEPENDS ${BOOST_TARGETS})
