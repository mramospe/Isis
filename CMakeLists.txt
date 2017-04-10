###############################################################################
#
#  AUTHOR: Miguel Ramos Pernas
#  e-mail: miguel.ramos.pernas@cern.ch
#
#  Last update: 10/04/2017
#
#  Description:
#
#  The compilation can be performed in the same directory as the source code
#  or in another one. If the flag BUILD_EXAMPLES is set to ON, all the cpp
#  examples will be built.
#
###############################################################################


#----------------------------------------------------------------------------
# Sets the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)
project(Isis VERSION 3.1)

#----------------------------------------------------------------------------
# C++ compiler flags
set(CMAKE_CXX_FLAGS "-Wall -O2 -std=c++11 -pthread -Wno-unused-local-typedefs")

#----------------------------------------------------------------------------
# Suffix for the shared libraries
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
set(ROOT_LIB_SET
  Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics
  MathCore Thread EG Html RooFitCore Minuit RooFit PyROOT)
find_package(ROOT REQUIRED COMPONENTS ${ROOT_LIB_SET})
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Set the installation directories
set(INSTALL_LIB_DIR      lib      CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR      bin      CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR  include  CACHE PATH "Installation directory for header files")
set(INSTALL_PYTHON_DIR   python   CACHE PATH "Installation directory for python files")
set(INSTALL_EXAMPLES_DIR examples CACHE PATH "Installation directory for examples")
set(INSTALL_SCRIPTS_DIR  scripts  CACHE PATH "Installation directory for scripts")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR}
  CACHE PATH "Installation directory for CMake files")
foreach(dir LIB BIN INCLUDE PYTHON EXAMPLES SCRIPTS CMAKE)
  set(var INSTALL_${dir}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_BINARY_DIR}/${${var}}")
  endif()
endforeach()

#----------------------------------------------------------------------------
# Initialize the targets
set(PROJECT_TARGETS ${PROJECT_NAME}Targets)

#============================================================================
# FUNCTIONS
#----------------------------------------------------------------------------
# This function creates a directory if it does not exist and copies all its
# contents. If the output directory already exists, the files that are not
# present or that are outdated are replaced.
function(copy_directory inputdir outputdir)
  
  file(MAKE_DIRECTORY ${outputdir})
  
  file(GLOB files ${inputdir}/*)
  foreach(file ${files})
    if( IS_DIRECTORY ${file} )
      get_filename_component(name ${file} NAME)
      copy_directory(${file} ${outputdir}/${name})
    else()
      configure_file(${file} ${outputdir} @ONLY)
    endif()
  endforeach()
  
endfunction()
#----------------------------------------------------------------------------
# This function defines the general way to compile the code of a package in
# the project
function(build_package libname deps)
  
  set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  
  file(GLOB files ${SOURCE_DIR}/*.h)
  set(headers)
  foreach(file ${files})
    configure_file(${file} ${INSTALL_INCLUDE_DIR})
    string(REPLACE ${SOURCE_DIR} ${INSTALL_INCLUDE_DIR} head ${file})
    list(APPEND headers ${head}})
  endforeach()
  
  include_directories(${INSTALL_INCLUDE_DIR})
  file(GLOB source ${SOURCE_DIR}/*.cpp)
  set(${PKG_NAME} ${INSTALL_LIB_DIR}/${libname})
  add_library(${libname} SHARED ${source})
  if(deps)
    add_dependencies(${libname} ${deps})
  endif()
  set_target_properties(${libname} PROPERTIES PUBLIC_HEADER "${headers}")
  
  set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${libname} PARENT_SCOPE)
  
  install(TARGETS ${libname}
    EXPORT ${PROJECT_TARGETS}
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev)
  
endfunction()
#============================================================================

#----------------------------------------------------------------------------
# Makes the directory for the include files
set(INSTALL_INCLUDE_DIR ${PROJECT_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${INSTALL_INCLUDE_DIR})
include_directories(${INSTALL_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Creates the output library directory
file(MAKE_DIRECTORY ${INSTALL_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INSTALL_LIB_DIR})
message("-- Output library directory set to: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

#----------------------------------------------------------------------------
# Adds the different main packages of the project. The list of libraries is
# reversed to correctly solve the dependencies. The python subdirectory
# depends on the project libraries, so it must be added after the list is
# reversed.
set(PROJECT_LIBRARIES)
add_subdirectory(general)
add_subdirectory(analysis)
list(REVERSE PROJECT_LIBRARIES)
add_subdirectory(python)

#----------------------------------------------------------------------------
# Generate two lists with the libraries from Root and this project prepended
# by "-l" and ordered following the dependencies
function(format_lib_list var prefix)
  set(list)
  foreach(f ${ARGN})
    list(APPEND list "${prefix}${f}")
  endforeach(f)
  string(REPLACE ";" " " list "${list}")
  set(${var} "${list}" PARENT_SCOPE)
endfunction(format_lib_list)

format_lib_list(ROOT_LIBRARIES_TO_LD "-l" ${ROOT_LIB_SET})
format_lib_list(PROJECT_LIBRARIES_TO_LD "-l" ${PROJECT_LIBRARIES})

#----------------------------------------------------------------------------
# The python folder is the only one which is not entirely copied
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  copy_directory(${PROJECT_SOURCE_DIR}/scripts ${INSTALL_SCRIPTS_DIR})
  copy_directory(${PROJECT_SOURCE_DIR}/examples ${INSTALL_EXAMPLES_DIR})
  copy_directory(${PROJECT_SOURCE_DIR}/bin ${INSTALL_BIN_DIR})
endif()

#----------------------------------------------------------------------------
# Build the example files if the option BUILD_EXAMPLES is set to ON
if(BUILD_EXAMPLES)
  if(${BUILD_EXAMPLES} STREQUAL "ON")
    message("-- The examples will be built")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${INSTALL_EXAMPLES_DIR})
    file(GLOB examples ${INSTALL_EXAMPLES_DIR}/*.cpp)
    foreach(file ${examples})
      string(REPLACE ".cpp" ".out" compfile ${file})
      string(REPLACE ${INSTALL_EXAMPLES_DIR}/ "" compfile ${compfile})
      add_executable(${compfile} ${file})
      add_dependencies(${compfile} ${PROJECT_LIBRARIES})
      target_link_libraries(${compfile} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
    endforeach()
  else()
    message("-- The examples will not be built")
  endif()
endif()

#----------------------------------------------------------------------------
# Generate the configuration files to build other projects with these headers
# and libraries

# Set the name of this project
export(PACKAGE ${PROJECT_NAME})

# Configuration files
set(PROJECT_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(PROJECT_CMAKE_CONFIG ${PROJECT_NAME}Config.cmake)
set(PROJECT_CMAKE_CONFIG_VERSION ${PROJECT_NAME}ConfigVersion.cmake)

# Libraries are set as targets
export(TARGETS ${PROJECT_LIBRARIES}
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_TARGETS}.cmake")

# Create the Config.cmake and ConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_BINARY_DIR}" "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIR "${INSTALL_INCLUDE_DIR}")
configure_file(${PROJECT_CMAKE_DIR}/${PROJECT_CMAKE_CONFIG}.in
  "${PROJECT_BINARY_DIR}/${PROJECT_CMAKE_CONFIG}" @ONLY)

# ... for both
configure_file(${PROJECT_CMAKE_DIR}/${PROJECT_CMAKE_CONFIG_VERSION}.in
  "${PROJECT_BINARY_DIR}/${PROJECT_CMAKE_CONFIG_VERSION}" @ONLY)

# Install the Config.cmake and ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_CMAKE_CONFIG}"
  "${PROJECT_BINARY_DIR}/${PROJECT_CMAKE_CONFIG_VERSION}"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT ${PROJECT_TARGETS} DESTINATION "${CMAKE_BINARY_DIR}" COMPONENT dev)
