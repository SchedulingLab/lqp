cmake_minimum_required(VERSION 3.0)

project(LQP
  VERSION 0.1.0
  LANGUAGES CXX
)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

option(LQP_BUILD_DOCUMENTATION "Build documentation (needs Doxygen)" OFF)

include(GNUInstallDirs)
set(LQP_DATADIR ${CMAKE_INSTALL_FULL_DATADIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(GLPK REQUIRED)

add_library(lqp0
  library/Expr.cc
  library/Inequality.cc
  library/Instance.cc
  library/Problem.cc
  library/Solver.cc
  library/Variable.cc

  library/solvers/GlpkSolver.cc
)

target_compile_features(lqp0
  PUBLIC
    cxx_std_17
)

target_include_directories(lqp0
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(lqp0
  PRIVATE
    GLPK::GLPK
)

set_target_properties(lqp0
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(
  TARGETS lqp0
  EXPORT lqpConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  EXPORT lqpConfig
  NAMESPACE lqp::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lpq"
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/lqp"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


# examples

add_executable(glpk_example
  examples/glpk_example.cc
)

target_link_libraries(glpk_example
  PRIVATE
    lqp0
)
