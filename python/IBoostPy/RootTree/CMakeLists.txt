#----------------------------------------------------------------------------
# Compile the specified boost package
project(RootTree)
self_build_boost_package(${PROJECT_NAME})
set(BOOST_TARGETS ${BOOST_TARGETS} PARENT_SCOPE)