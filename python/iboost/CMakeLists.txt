#----------------------------------------------------------------------------
# Name of the project
set(PKG_NAME iboost)

#----------------------------------------------------------------------------
# Look for the Boost and python libraries
find_package(PythonLibs REQUIRED)

#----------------------------------------------------------------------------
# Set of libraries linked to those created here
set(BOOSTPY_LIBS
  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Function to compile each of the boost packages
function(build_boost_package libname)
  file(GLOB source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
  include_directories(${INSTALL_INCLUDE_DIR})
  include_directories(${PYTHON_INCLUDE_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
  set(${PKG_NAME} ${BOOST_OUTPUT_LIBRARY_DIR}/${libname})
  add_library(${libname} SHARED ${source})
  set(libs iboost ${BOOSTPY_LIBS})
  target_link_libraries(${libname} ${libs})
  set_target_properties(${libname} PROPERTIES PREFIX "")
  add_dependencies(${libname} iboost Analysis General)
  set(IBOOST_TARGETS ${IBOOST_TARGETS} ${libname} PARENT_SCOPE)
endfunction()

#----------------------------------------------------------------------------
# Compile common code to be used by the wrappers
file(GLOB source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${INSTALL_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_library(${PKG_NAME} SHARED ${source})
set(libs ${BOOSTPY_LIBS})
target_link_libraries(${PKG_NAME} ${libs})
add_dependencies(${libname} Analysis General)

#----------------------------------------------------------------------------
# Define the output location of the compiled libraries
file(MAKE_DIRECTORY ${PYBINARY}/${PKG_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PYBINARY}/${PKG_NAME})

#----------------------------------------------------------------------------
# Specify the custom boost packages
add_subdirectory(rootio)
add_subdirectory(general)
add_subdirectory(analysis)

#----------------------------------------------------------------------------
# Pass the boost targets to the parent scope. This must be done after all
# the targets are set.
set(IBOOST_TARGETS ${IBOOST_TARGETS} PARENT_SCOPE)
