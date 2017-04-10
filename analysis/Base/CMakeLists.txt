#----------------------------------------------------------------------------
# Here the package is defined. One has to specify both the library name and
# the dependences through the project.
set(PKG_NAME Base)
set(LIB_NAME Analysis)
set(PKG_DEPS General)

#----------------------------------------------------------------------------
# This lines must be copied for each general-compiled package
build_package(${LIB_NAME} ${PKG_DEPS})
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} PARENT_SCOPE)