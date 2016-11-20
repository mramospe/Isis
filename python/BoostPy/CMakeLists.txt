#----------------------------------------------------------------------------
# Name of the project
project(BoostPy)

#----------------------------------------------------------------------------
# Look for the Boost and python libraries
find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibs REQUIRED)

#----------------------------------------------------------------------------
# Function to compile each of the boost packages
function(self_build_boost_package libname)
  file(GLOB source ${PROJECT_SOURCE_DIR}/source/*.cpp)
  include_directories(${BINARY_INCLUDE_DIR})
  include_directories(${PYTHON_INCLUDE_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR}/include)
  set(${PROJECT_NAME} ${BOOST_OUTPUT_LIBRARY_DIR}/${libname}.so)
  add_library(${libname} SHARED ${source})
  set(libs ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${PROJECT_LIBRARIES} ${ROOT_LIBRARIES})
  target_link_libraries(${libname} ${libs})
  set_target_properties(${libname} PROPERTIES PREFIX "")
  add_dependencies(${libname} Analysis General)
  set(BOOST_TARGETS ${BOOST_TARGETS} ${libname} PARENT_SCOPE)
endfunction()

#----------------------------------------------------------------------------
# Define the output location of the compiled libraries
self_make_directory(${PYBINARY}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PYBINARY}/${PROJECT_NAME})

#----------------------------------------------------------------------------
# Specify the custom boost packages
add_subdirectory(RootTree)
add_subdirectory(PyGeneral)

#----------------------------------------------------------------------------
# Pass the boost targets to the parent scope. This must be done after all
# the targets are set.
set(BOOST_TARGETS ${BOOST_TARGETS} PARENT_SCOPE)