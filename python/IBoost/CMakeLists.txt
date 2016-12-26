#----------------------------------------------------------------------------
# Name of the project
project(IBoost)

#----------------------------------------------------------------------------
# Look for the Boost and python libraries
find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibs REQUIRED)

#----------------------------------------------------------------------------
# Set of libraries linked to those created here
set(BOOSTPY_LIBS
  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Function to compile each of the boost packages
function(self_build_boost_package libname)
  file(GLOB source ${PROJECT_SOURCE_DIR}/src/*.cpp)
  include_directories(${BINARY_INCLUDE_DIR})
  include_directories(${PYTHON_INCLUDE_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR}/src)
  set(${PROJECT_NAME} ${BOOST_OUTPUT_LIBRARY_DIR}/${libname})
  add_library(${libname} SHARED ${source})
  set(libs IBoost ${BOOSTPY_LIBS})
  target_link_libraries(${libname} ${libs})
  set_target_properties(${libname} PROPERTIES PREFIX "")
  add_dependencies(${libname} IBoost Analysis General)
  set(BOOST_TARGETS ${BOOST_TARGETS} ${libname} PARENT_SCOPE)
endfunction()

#----------------------------------------------------------------------------
# Compile common code to be used by the wrappers
file(GLOB source ${PROJECT_SOURCE_DIR}/src/*.cpp)
include_directories(${BINARY_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
add_library(${PROJECT_NAME} SHARED ${source})
set(libs ${BOOSTPY_LIBS})
target_link_libraries(${PROJECT_NAME} ${libs})
add_dependencies(${libname} Analysis General)

#----------------------------------------------------------------------------
# Define the output location of the compiled libraries
self_make_directory(${PYBINARY}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PYBINARY}/${PROJECT_NAME})

#----------------------------------------------------------------------------
# Specify the custom boost packages
add_subdirectory(RootTree)
add_subdirectory(PyGeneral)
add_subdirectory(PyAnalysis)

#----------------------------------------------------------------------------
# Pass the boost targets to the parent scope. This must be done after all
# the targets are set.
set(BOOST_TARGETS ${BOOST_TARGETS} PARENT_SCOPE)