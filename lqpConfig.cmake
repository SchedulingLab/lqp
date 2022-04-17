include (CMakeFindDependencyMacro)

get_filename_component (LQP_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
include("${LQP_CMAKE_DIR}/lqpTargets.cmake")

list(APPEND CMAKE_MODULE_PATH "${LQP_CMAKE_DIR}/cmake")

if(NOT BUILD_SHARED_LIBS)
  find_dependency(GLPK)
  # find_dependency(Gurobi)
endif ()

set(LQP_FOUND TRUE)

include("${LQP_CMAKE_DIR}/lqpConfigVersion.cmake")

message(STATUS "Found lqp (version \"${PACKAGE_VERSION}\") in ${CMAKE_CURRENT_LIST_DIR}")
