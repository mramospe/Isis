#----------------------------------------------------------------------------
# Compile the specified boost package
project(general)
build_boost_package(${PROJECT_NAME})
set(IBOOST_TARGETS ${IBOOST_TARGETS} PARENT_SCOPE)
