###############################################################################
#                                                                             #
#  AUTHOR: Miguel Ramos Pernas                                                #
#  e-mail: miguel.ramos.pernas@cern.ch                                        #
#                                                                             #
#  Last update: 19/04/2016                                                    #
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
execute_process(
  COMMAND ${PROJECT_SOURCE_DIR}/bin/./Isis-config --rlibs
  OUTPUT_VARIABLE ROOT_LIBRARIES
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

#----------------------------------------------------------------------------
# Sets the compiler flags
execute_process(
  COMMAND ${PROJECT_SOURCE_DIR}/bin/./Isis-config --cflags
  OUTPUT_VARIABLE CMAKE_CXX_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

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
# This function defines the general way to compile the code of a package in
# the project. In spite of this, one can define another way to perform this
# task, as long as the library is correctly exported.
function(self_build_package libname deps)
  file(GLOB files ${PROJECT_SOURCE_DIR}/include/*)
  foreach(file ${files})
    self_copy_file(${file} ${BINARY_INCLUDE_DIR})
  endforeach()
  include_directories(${BINARY_INCLUDE_DIR})
  file(GLOB source ${PROJECT_SOURCE_DIR}/source/*.cpp)
  set(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/lib/lib${libname}.so)
  add_library(${libname} SHARED ${source})
  if(deps)
    add_dependencies(${libname} ${deps})
  endif()
  set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${libname} PARENT_SCOPE)
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
# Makes the directory for the include files
set(BINARY_INCLUDE_DIR ${PROJECT_BINARY_DIR}/include)
self_make_directory(${BINARY_INCLUDE_DIR})
include_directories(${BINARY_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Creates the output library directory
self_make_directory(${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
message("-- Output library directory set to: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

#----------------------------------------------------------------------------
# Adds the different main packages of the project. The list of libraries is
# reversed to avoid linking problems.
set(PROJECT_LIBRARIES)
add_subdirectory(General)
add_subdirectory(Analysis)
add_subdirectory(python)
list(REVERSE PROJECT_LIBRARIES)

#----------------------------------------------------------------------------
# Specifies the scripts of the tools to be compiled, and copies the .C and
# .py files to the folder
self_make_directory(${PROJECT_BINARY_DIR}/Tools)
set(TOOLS_DIR ${PROJECT_SOURCE_DIR}/Tools)
self_make_directory(${PROJECT_BINARY_DIR}/Tools)
file(GLOB ncomptools ${TOOLS_DIR}/*.C ${TOOLS_DIR}/*.py)
foreach(path ${ncomptools})
  string(REPLACE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} script ${path})
  configure_file(${path} ${script} COPYONLY)
endforeach()

file(GLOB srctools ${TOOLS_DIR}/*.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Tools)
foreach(tool ${srctools})
  string(REPLACE ".cpp" ".out" comptool ${tool})
  string(REPLACE ${TOOLS_DIR}/ "" comptool ${comptool})
  add_executable(${comptool} ${tool})
  add_dependencies(${comptool} ${PROJECT_LIBRARIES})
  target_link_libraries(${comptool} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
endforeach()

#----------------------------------------------------------------------------
# Copies the example files. If the option BUILD_EXAMPLES is ON, the scripts
# will be compiled.
if(BUILD_EXAMPLES)
  if(${BUILD_EXAMPLES} STREQUAL "ON")
    message("-- The examples will be built")
    set(EXAMPLES_DIR ${PROJECT_BINARY_DIR}/Examples)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_DIR})
    file(GLOB examples ${EXAMPLES_DIR}/*.cpp)
    foreach(file ${examples})
      string(REPLACE ".cpp" ".out" compfile ${file})
      string(REPLACE ${EXAMPLES_DIR}/ "" compfile ${compfile})
      add_executable(${compfile} ${file})
      add_dependencies(${compfile} ${PROJECT_LIBRARIES})
      target_link_libraries(${compfile} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
    endforeach()
  else()
    message("-- The examples will not be built")
  endif()
endif()