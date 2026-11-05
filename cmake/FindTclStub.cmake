# Finds the Tcl headers and the Tcl stub library.
#
#   TclStub_FOUND     = the headers and stub library were found
#   TCL_INCLUDE_PATH  = directory containing tcl.h
#   TCL_STUB_LIBRARY  = path to the Tcl stub library
#   TCL_VERSION       = Tcl patch level read from tcl.h
#   TCL_TCLSH         = tclsh matching TCL_VERSION, if one is installed
#
# Set TCL_INCLUDE_DIR and TCL_LIBRARY_DIR to search a specific Tcl install,
# or set TCL_INCLUDE_PATH and TCL_STUB_LIBRARY directly.

find_path(TCL_INCLUDE_PATH
  NAMES tcl.h
  HINTS ${TCL_INCLUDE_DIR}
  PATH_SUFFIXES tcl9.1 tcl9.0 tcl8.6
)

get_filename_component(TCL_INCLUDE_PATH_PARENT "${TCL_INCLUDE_PATH}" DIRECTORY)

if(EXISTS "${TCL_INCLUDE_PATH}/tcl.h")
  file(STRINGS "${TCL_INCLUDE_PATH}/tcl.h" TCL_PATCH_LEVEL_LINE
       REGEX "^#[ \t]*define[ \t]+TCL_PATCH_LEVEL[ \t]+\"[^\"]+\"")
  string(REGEX REPLACE ".*\"([^\"]+)\".*" "\\1" TCL_VERSION "${TCL_PATCH_LEVEL_LINE}")
endif()

# Prefer the stub archive matching the selected headers.
if(TCL_VERSION VERSION_LESS 9.0)
  set(TCL_STUB_NAMES tclstub8.6 tclstub86 tclstub)
else()
  set(TCL_STUB_NAMES tclstub tclstub9.1 tclstub91 tclstub9.0 tclstub90)
endif()
find_library(TCL_STUB_LIBRARY
  NAMES ${TCL_STUB_NAMES} NAMES_PER_DIR
  HINTS ${TCL_LIBRARY_DIR} "${TCL_INCLUDE_PATH_PARENT}/lib"
  PATH_SUFFIXES tcl9.1 tcl9.0 tcl8.6
)

if(TCL_VERSION MATCHES "^([0-9]+)\\.([0-9]+)")
  set(TCL_TCLSH_NAMES tclsh${CMAKE_MATCH_1}.${CMAKE_MATCH_2} tclsh${CMAKE_MATCH_1}${CMAKE_MATCH_2}
      tclsh${CMAKE_MATCH_1}${CMAKE_MATCH_2}t)
endif()

find_program(TCL_TCLSH
  NAMES ${TCL_TCLSH_NAMES} tclsh NAMES_PER_DIR
  HINTS "${TCL_INCLUDE_PATH_PARENT}/bin"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TclStub
  REQUIRED_VARS TCL_STUB_LIBRARY TCL_INCLUDE_PATH
  VERSION_VAR TCL_VERSION
)

mark_as_advanced(TCL_INCLUDE_PATH TCL_STUB_LIBRARY TCL_TCLSH)
