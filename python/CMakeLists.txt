#----------------------------------------------------------------------------
# Defines the way to manage the python source. It will be translated to
# byte-code using the script compile.py.
project(Isis-python)
set(PYSOURCE ${PROJECT_SOURCE_DIR})
set(PYBINARY ${PROJECT_BINARY_DIR}/Isis)
add_custom_target(Isis-python ALL python ${PYSOURCE}/compile.py ${PYSOURCE}/Isis ${PYBINARY})
