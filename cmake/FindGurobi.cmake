# Copyright (c) 2022 Julien Bernard
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

set(GUROBI_SEARCH_PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /opt/local
  /opt
)

find_library(GUROBI_CPP_LIBRARY
  NAMES
    gurobi_c++
  HINTS
    $ENV{GUROBIDIR}
  PATH_SUFFIXES
    lib
  PATHS
    ${GUROBI_SEARCH_PATHS}
)

find_path(GUROBI_CPP_INCLUDE_DIR gurobi_c++.h
  HINTS
    $ENV{GUROBIDIR}
  PATH_SUFFIXES
    include
  PATHS
    ${GUROBI_SEARCH_PATHS}
)

find_library(GUROBI_C_LIBRARY
  NAMES
    "gurobi$ENV{GUROBI_VERSION}"
  HINTS
    $ENV{GUROBIDIR}
  PATH_SUFFIXES
    lib
  PATHS
    ${GUROBI_SEARCH_PATHS}
)

find_path(GUROBI_C_INCLUDE_DIR gurobi_c.h
  HINTS
    $ENV{GUROBIDIR}
  PATH_SUFFIXES
    include
  PATHS
    ${GUROBI_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args("Gurobi" DEFAULT_MSG
  GUROBI_CPP_LIBRARY
  GUROBI_CPP_INCLUDE_DIR
  GUROBI_C_LIBRARY
  GUROBI_C_INCLUDE_DIR
)

if(GUROBI_FOUND)
  message(STATUS "Gurobi include directory: ${GUROBI_CPP_INCLUDE_DIR}")
endif()

if(GUROBI_FOUND AND NOT TARGET Gurobi::Gurobi)
  add_library(Gurobi::Gurobi_CPP UNKNOWN IMPORTED)
  set_target_properties(Gurobi::Gurobi_CPP
    PROPERTIES
      IMPORTED_LOCATION "${GUROBI_CPP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GUROBI_CPP_INCLUDE_DIR}"
  )

  add_library(Gurobi::Gurobi_C UNKNOWN IMPORTED)
  set_target_properties(Gurobi::Gurobi_C
    PROPERTIES
      IMPORTED_LOCATION "${GUROBI_C_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GUROBI_C_INCLUDE_DIR}"
  )

  add_library(Gurobi::Gurobi INTERFACE IMPORTED)
  set_property(TARGET Gurobi::Gurobi
    PROPERTY
      INTERFACE_LINK_LIBRARIES Gurobi::Gurobi_CPP Gurobi::Gurobi_C
  )
endif()

mark_as_advanced(
  GUROBI_CPP_LIBRARY
  GUROBI_CPP_INCLUDE_DIR
  GUROBI_C_LIBRARY
  GUROBI_C_INCLUDE_DIR
)
