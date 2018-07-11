# Find module for Ppconsul.
# Variables:
#  - PPCONSUL_FOUND: TRUE if found
#  - PPCONSUL_LIBRARIES: Ppconsul libraries to link
#  - PPCONSUL_INCLUDE_DIR: include dir
include(FindPackageHandleStandardArgs)

find_library(PPCONSUL_LIBRARIES
             NAMES ppconsul
             HINTS ${PPCONSUL_DIR} ${PPCONSUL_LIBRARY_DIRS}/..
             PATH_SUFFIXES lib)

find_path(PPCONSUL_INCLUDE_DIR
          NAMES ppconsul/ppconsul.h
          HINTS ${PPCONSUL_DIR} ${PPCONSUL_INCLUDE_DIRS}/..
          PATH_SUFFIXES include)

find_package_handle_standard_args(Ppconsul DEFAULT_MSG
                                  PPCONSUL_LIBRARIES
                                  PPCONSUL_INCLUDE_DIR)

message(STATUS "FOUND PPCONSUL LIBS ==> ${PPCONSUL_LIBRARIES}")
