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

set(GLPK_SEARCH_PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /opt/local
  /opt
)

find_library(GLPK_LIBRARY
  NAMES
    glpk
  HINTS
    $ENV{GLPKDIR}
  PATH_SUFFIXES
    lib
  PATHS
    ${GLPK_SEARCH_PATHS}
)

find_path(GLPK_INCLUDE_DIR glpk.h
  HINTS
    $ENV{GLPKDIR}
  PATH_SUFFIXES
    include
  PATHS
    ${GLPK_SEARCH_PATHS}
)

if(NOT GLPK_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find GLPK include directory.")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args("GLPK" DEFAULT_MSG
  GLPK_LIBRARY
  GLPK_INCLUDE_DIR
)

message(STATUS "GLPK include directory: ${GLPK_INCLUDE_DIR}")
message(STATUS "GLPK library: ${GLPK_LIBRARY}")

mark_as_advanced(
  GLPK_LIBRARY
  GLPK_INCLUDE_DIR
)

if(NOT TARGET GLPK::GLPK)
  add_library(GLPK::GLPK UNKNOWN IMPORTED)
  set_target_properties(GLPK::GLPK
    PROPERTIES
      IMPORTED_LOCATION "${GLPK_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GLPK_INCLUDE_DIR}"
  )
endif()
