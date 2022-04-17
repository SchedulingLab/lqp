cmake_minimum_required(VERSION 3.0)

project(LQP
  VERSION 0.1.0
  LANGUAGES CXX
)

# global settings

include(GNUInstallDirs)
include(GenerateExportHeader)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# options

option(BUILD_SHARED_LIBS "Build the library as a shared library" ON)
option(LQP_WITH_GUROBI "Build with Gurobi support (if available)" OFF)

# dependencies

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(GLPK REQUIRED)

if(LQP_WITH_GUROBI)
  find_package(Gurobi)

  if(Gurobi_FOUND)
    set(LQP_HAS_GUROBI 1)
  endif()
endif()

# library

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/library/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_library(lqp0
  library/Expr.cc
  library/Inequality.cc
  library/Problem.cc
  library/Solution.cc
  library/Solver.cc
  library/Variable.cc

  library/solvers/GlpkSolver.cc
  library/solvers/GurobiSolver.cc
)

generate_export_header(lqp0
  BASE_NAME LQP
  EXPORT_MACRO_NAME LQP_API
  EXPORT_FILE_NAME "lqp/Api.h"
  STATIC_DEFINE STATIC
)

target_compile_features(lqp0
  PUBLIC
    cxx_std_17
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(lqp0
    PUBLIC
      LQP_STATIC
  )
endif()

set_target_properties(lqp0
  PROPERTIES
    EXPORT_NAME "lqp"
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(lqp0
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/lqp>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(lqp0
  PRIVATE
    GLPK::GLPK
)

if(Gurobi_FOUND)
  target_link_libraries(lqp0
    PRIVATE
      Gurobi::Gurobi
  )
endif()

if(NOT BUILD_SHARED_LIBS)
  set_property(TARGET lqp0 PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

# install

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/lqp"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  TARGETS lqp0
  EXPORT lqpTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lqp/Api.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lqp"
)

install(
  EXPORT lqpTargets
  NAMESPACE lqp::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lqp"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lqpConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/lqpConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lqpConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lqp"
)

install(
  FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindGLPK.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindGurobi.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lqp/cmake"
)

# examples

add_executable(glpk_example
  examples/glpk_example.cc
)

target_link_libraries(glpk_example
  PRIVATE
    lqp0
)
