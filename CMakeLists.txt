###############################################################################
#                                                                             #
#  AUTHOR: Miguel Ramos Pernas                                                #
#  e-mail: miguel.ramos.pernas@cern.ch                                        #
#                                                                             #
#  Last update: 16/02/2016                                                    #
#                                                                             #
#  Description:                                                               #
#                                                                             #
#  The compilation can be performed in the same directory as the source code  #
#  or in another one. If the flag BUILD_EXAMPLES is set to ON, all the cpp    #
#  examples will be built.                                                    #
#                                                                             #
###############################################################################


#----------------------------------------------------------------------------
# Sets the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Isis)
set(libsuffix .so)

#----------------------------------------------------------------------------
# The root directory can be specified via -DUSER_ROOT_PATH=path when running
# cmake. Otherwise it will be set as the current $ROOTSYS path.
if(NOT USER_ROOT_PATH)
  execute_process(
    COMMAND root-config --prefix
    OUTPUT_VARIABLE USER_ROOT_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

#----------------------------------------------------------------------------
# The path to the FindROOT.cmake file is specified here
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${USER_ROOT_PATH}/etc/cmake)
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})
file(GLOB ROOT_LIBRARIES ${ROOT_LIBRARY_DIR}/*.so)

#----------------------------------------------------------------------------
# Defines two functions that copy or create a given directory/file only if it
# does not exist. In the case of files it will also be copied if it is newer
# than that in the specified path.
function(self_copy_file file, path)
  get_filename_component(name ${file} NAME)
  if(NOT EXISTS ${path}/${name} OR ${file} IS_NEWER_THAN ${path}/${name})
    execute_process(COMMAND cp ${file} ${path}/.)
  endif()
endfunction()
function(self_make_directory dir)
  if(NOT EXISTS ${dir})
    file(MAKE_DIRECTORY ${dir})
  endif()
endfunction()

#----------------------------------------------------------------------------
# The python folder is created only if the source directory is equal to the
# binary directory. The same happens with the copy of the Examples folder and
# the bin directory.
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  self_make_directory(${PROJECT_BINARY_DIR}/python)
  self_make_directory(${PROJECT_BINARY_DIR}/python/Isis)
  execute_process(COMMAND cp -r ${PROJECT_SOURCE_DIR}/Examples .)
  self_make_directory(${PROJECT_BINARY_DIR}/bin)
  file(GLOB files ${PROJECT_SOURCE_DIR}/bin/*)
  foreach(infile ${files})
    string(REPLACE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} outfile ${infile})
    configure_file(${infile} ${outfile} COPYONLY)
  endforeach()
endif()

#----------------------------------------------------------------------------
# Defines the way to compile the python source
set(PYSOURCE ${PROJECT_SOURCE_DIR}/python )
set(PYBINARY ${PROJECT_BINARY_DIR}/python/Isis)
add_custom_target(Isis-python ALL python ${PYSOURCE}/compile.py ${PYSOURCE}/Isis ${PYBINARY})

#----------------------------------------------------------------------------
# Defines the different Analysis packages
set(ANALYSIS_EXTRA_PKGS Weights AdaptiveBinning RooFitExt)
set(ANALYSIS_EXTRA_LIBS Wts     AdBin           RFExt    )

#----------------------------------------------------------------------------
# Makes the directories for the include files
set(BINARY_INCLUDE_DIR ${PROJECT_BINARY_DIR}/include)
self_make_directory(${BINARY_INCLUDE_DIR})
foreach(pkg ${ANALYSIS_EXTRA_PKGS})
  self_make_directory(${BINARY_INCLUDE_DIR}/${pkg})
  file(GLOB files ${PROJECT_SOURCE_DIR}/Analysis/${pkg}/include/*)
  foreach(file ${files})
    self_copy_file(${file} ${BINARY_INCLUDE_DIR}/${pkg})
  endforeach()
endforeach()

#----------------------------------------------------------------------------
# Adds the include files of the main packages
file(GLOB files ${PROJECT_SOURCE_DIR}/Analysis/Base/include/*)
foreach(file ${files})
  self_copy_file(${file} ${BINARY_INCLUDE_DIR})
endforeach()
file(GLOB files ${PROJECT_SOURCE_DIR}/General/include/*)
foreach(file ${files})
  self_copy_file(${file} ${BINARY_INCLUDE_DIR})
endforeach()

#----------------------------------------------------------------------------
# Sets the path to the include directory of the project
include_directories(${BINARY_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Creates the output library directory
self_make_directory(${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
message("-- Output library directory set to: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

#----------------------------------------------------------------------------
# Sets the compiler flags
execute_process(
  COMMAND ${PROJECT_SOURCE_DIR}/bin/./Isis-config --cflags
  OUTPUT_VARIABLE CMAKE_CXX_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

#----------------------------------------------------------------------------
# Defines how to generate the different libraries of the Isis project

# Specifies the source for the general package
file(GLOB source ${PROJECT_SOURCE_DIR}/General/source/*.cpp)
set(General ${PROJECT_BINARY_DIR}/lib/libGeneral.so)
add_library(General SHARED ${source})

# Initializes the list of libraries of the project
set(PROJECT_LIBRARIES General)

# Specifies the different analysis folders and library names
set(ANALYSIS_PKG_NAMES Base     ${ANALYSIS_EXTRA_PKGS})
set(ANALYSIS_LIB_NAMES Analysis ${ANALYSIS_EXTRA_LIBS})
list(LENGTH ANALYSIS_PKG_NAMES length)
math(EXPR length "${length} - 1")
foreach(index RANGE ${length})
  list(GET ANALYSIS_PKG_NAMES ${index} folder)
  list(GET ANALYSIS_LIB_NAMES ${index} lib )
  file(GLOB source ${PROJECT_SOURCE_DIR}/Analysis/${folder}/source/*.cpp)
  add_library(${lib} SHARED ${source})
  list(APPEND PROJECT_LIBRARIES ${lib})
endforeach()

# Specifies the dependencies for each library
add_dependencies(Analysis General)
add_dependencies(Wts      Analysis)
add_dependencies(AdBin    Analysis)
add_dependencies(RFExt    AdBin)

# Reverses the list to properly order de dependences
list(REVERSE PROJECT_LIBRARIES)

#----------------------------------------------------------------------------
# Specifies the scripts of the tools to be compiled, and copies the .C and
# .py files to the folder
self_make_directory(${PROJECT_BINARY_DIR}/Tools)
set(TOOLS_DIR ${PROJECT_SOURCE_DIR}/Tools)
file(GLOB srctools ${TOOLS_DIR}/*.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Tools)
foreach(tool ${srctools})
  string(REPLACE ".cpp" ".out" comptool ${tool})
  string(REPLACE ${TOOLS_DIR}/ "" comptool ${comptool})
  add_executable(${comptool} ${tool})
  target_link_libraries(${comptool} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
  add_dependencies(${comptool} ${PROJECT_LIBRARIES})
  add_dependencies(Isis-python ${comptool})
endforeach()
file(GLOB ncomptools ${TOOLS_DIR}/*.C ${TOOLS_DIR}/*.py)
foreach(path ${ncomptools})
  string(REPLACE ${PROJECT_SOURCE_DIR}/ "" script ${path})
  configure_file(${path} ${script} COPYONLY)
endforeach()

#----------------------------------------------------------------------------
# Copies the example files. If the option BUILD_EXAMPLES is ON, the scripts
# will be compiled.
if(BUILD_EXAMPLES)
  if(${BUILD_EXAMPLES} STREQUAL "ON")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Examples)
    set(EXAMPLES_DIR ${PROJECT_BINARY_DIR}/Examples)
    file(GLOB examples ${EXAMPLES_DIR}/*.cpp)
    foreach(file ${examples})
      string(REPLACE ".cpp" ".out" compfile ${file})
      string(REPLACE ${EXAMPLES_DIR}/ "" compfile ${compfile})
      add_executable(${compfile} ${file})
      target_link_libraries(${compfile} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
      add_dependencies(${compfile} Isis-python ${PROJECT_LIBRARIES})
    endforeach()
  endif()
endif()