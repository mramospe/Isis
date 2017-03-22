#----------------------------------------------------------------------------
# Here the way to compile the General package is specified
set(PKG_NAME General)
set(LIB_NAME General)
set(PKG_DEPS "NO")

#----------------------------------------------------------------------------
# This lines must be copied for each general-compiled package
build_package(${LIB_NAME} ${PKG_DEPS})
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} PARENT_SCOPE)
