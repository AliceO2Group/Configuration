# Find module for Ppconsul.
# Variables:
#  - Ppconsul_FOUND: TRUE if found
#  - Ppconsul_LIBRARIES: Ppconsul libraries to link
#  - Ppconsul_INCLUDE_DIR: include dir
include(FindPackageHandleStandardArgs)

find_library(Ppconsul_LIBRARY
  NAMES ppconsul
  HINTS ${Ppconsul_DIR} ${Ppconsul_LIBRARY_DIRS}/..
  PATH_SUFFIXES lib)

mark_as_advanced(Ppconsul_LIBRARY)

find_path(Ppconsul_INCLUDE_DIR
  NAMES ppconsul/ppconsul.h
  HINTS ${Ppconsul_DIR} ${Ppconsul_INCLUDE_DIRS}/..
  PATH_SUFFIXES include)

mark_as_advanced(Ppconsul)

find_package_handle_standard_args(Ppconsul DEFAULT_MSG
  Ppconsul_LIBRARY
  Ppconsul_INCLUDE_DIR)

if(Ppconsul_FOUND)
  set(Ppconsul_LIBRARIES ${Ppconsul_LIBRARY})
  set(Ppconsul_INCLUDE_DIRS ${Ppconsul_INCLUDE_DIR})

  # add target
  if(NOT TARGET Ppconsul::Ppconsul)
    add_library(Ppconsul::Ppconsul INTERFACE IMPORTED)
    set_target_properties(Ppconsul::Ppconsul PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${Ppconsul_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${Ppconsul_LIBRARY}"
    )
  endif()
endif()
