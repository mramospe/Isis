#----------------------------------------------------------------------------
# Here the package is defined. One has to specify both the library name and
# the dependences through the project.
project(Clustering)
set(LIB_NAME Cluster)
set(PKG_DEPS Analysis)

#----------------------------------------------------------------------------
# This lines must be copied for each general-compiled package
self_build_package(${LIB_NAME} ${PKG_DEPS})
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} PARENT_SCOPE)