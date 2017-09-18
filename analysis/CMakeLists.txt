#----------------------------------------------------------------------------
# Here the Analysis project is defined. Each folder placed in the same
# directory as this file is considered as a subpackage, and it is added to
# the compilation process.
set(PKG_NAME analysis)
file(GLOB pkgs ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(pkg ${pkgs})
  if(IS_DIRECTORY ${pkg})
    add_subdirectory(${pkg})
  endif()
endforeach()
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} PARENT_SCOPE)
