# Find module for Ppconsul.
# Variables:
#  - PPCONSUL_FOUND: TRUE if found
#  - PPCONSUL_LIBRARIES: Ppconsul libraries to link
#  - PPCONSUL_INCLUDE_DIR: include dir
include(FindPackageHandleStandardArgs)

find_library(_LIBPPCONSUL
             NAMES ppconsul
             HINTS ${PPCONSUL_DIR} ${PPCONSUL_LIBRARY_DIRS}/..
             PATH_SUFFIXES lib)
find_library(_LIBJSON11
             NAMES json11
             HINTS ${PPCONSUL_DIR} ${PPCONSUL_LIBRARY_DIRS}/..
             PATH_SUFFIXES lib)
find_path(PPCONSUL_INCLUDE_DIR
          NAMES ppconsul/ppconsul.h
          HINTS ${PPCONSUL_DIR} ${PPCONSUL_INCLUDE_DIRS}/..
          PATH_SUFFIXES include)

find_package_handle_standard_args(Ppconsul DEFAULT_MSG
                                  _LIBPPCONSUL
                                  PPCONSUL_INCLUDE_DIR)

set(PPCONSUL_LIBRARIES ${_LIBPPCONSUL})
if(${_LIBJSON11})
  set(PPCONSUL_LIBRARIES ${PPCONSUL_LIBRARIES};${_LIBJSON11})
endif()

unset(_LIBPPCONSUL)
unset(_LIBJSON11)
