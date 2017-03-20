###############################################################################
#
#  AUTHOR: Miguel Ramos Pernas
#  e-mail: miguel.ramos.pernas@cern.ch
#
#  Last update: 20/03/2017
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
# Set the installation directories
set(INSTALL_LIB_DIR      lib      CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR      bin      CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR  include  CACHE PATH "Installation directory for header files")
set(INSTALL_EXAMPLES_DIR examples CACHE PATH "Installation directory for examples")
set(INSTALL_SCRIPTS_DIR  scripts  CACHE PATH "Installation directory for scripts")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR}
  CACHE PATH "Installation directory for CMake files")
foreach(dir LIB BIN INCLUDE EXAMPLES SCRIPTS CMAKE)
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
      configure_file(${file} ${outputdir} COPYONLY)
    endif()
  endforeach()
  
endfunction()
#----------------------------------------------------------------------------
# This function defines the general way to compile the code of a package in
# the project. In spite of this, one can define another way to perform this
# task, as long as the library is correctly exported.
function(build_package libname deps)
  
  set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  
  file(GLOB files ${SOURCE_DIR}/*.h)
  set(headers)
  foreach(file ${files})
    configure_file(${file} ${INSTALL_INCLUDE_DIR} COPYONLY)
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
# The python folder is created only if the source directory is equal to the
# binary directory. The same happens with the copy of the Examples folder and
# the bin directory.
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/python/Isis)
  copy_directory(${PROJECT_SOURCE_DIR}/scripts ${INSTALL_SCRIPTS_DIR})
  copy_directory(${PROJECT_SOURCE_DIR}/examples ${INSTALL_EXAMPLES_DIR})
  copy_directory(${PROJECT_SOURCE_DIR}/bin ${INSTALL_BIN_DIR})
endif()

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
# reversed to avoid linking problems. The python subdirectory depends on the
# project libraries, so it must be added after the list is reversed.
set(PROJECT_LIBRARIES)
add_subdirectory(General)
add_subdirectory(Analysis)
list(REVERSE PROJECT_LIBRARIES)
add_subdirectory(python)

#----------------------------------------------------------------------------
# Copies the example files. If the option BUILD_EXAMPLES is ON, the scripts
# will be compiled.
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
