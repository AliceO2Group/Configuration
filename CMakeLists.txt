# CONFIGURATION CMAKELISTS.TXT
#
# @author  sylvain.chapeland@cern.ch

cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake) # main (top) cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir

# Set the name of your project here
project("Configuration")

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

## Git (and its revision)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
## GetGitRevisionDescription module to retreive branch and revision information from Git
## Starting with Git 1.9 the module will be part of official cMake distribution, until then it has to be
## part of the application
## The Git module will trigger a reconfiguration for each pull that will bring a new revision on the local repository
set (VCS_REVISION "-1")
if(GIT_FOUND)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    message(STATUS "GIT branch ${GIT_REFSPEC}")
    message(STATUS "GIT revision ${GIT_SHA1}")
    set (VCS_REVISION ${GIT_SHA1})
endif()

# C++11 needed
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++11 COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Mac needed variables (adapt according to your needs)
set(CMAKE_MACOSX_RPATH ON)
#set(CMAKE_SKIP_BUILD_RPATH FALSE)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

file(
  GLOB_RECURSE
  source_files
  src/*
  test/*
)

file(
  GLOB_RECURSE
  PUBLIC_HEADERS
  include/Configuration/*
)

include_directories (
  include
  src
)

add_library (
  Configuration-static STATIC
  src/Configuration.cxx
)

add_library (
  Configuration-shared SHARED
  src/Configuration.cxx
)

add_executable (
  testConfiguration
  test/testConfiguration.cxx
)

target_link_libraries(
  testConfiguration
  Configuration-shared
)

# Not sure why, but I need this to use the static lib from a shared lib. 
# Can be removed once we generate a proper shared lib.
add_definitions("-fPIC")

# Specify how/where to install this libraries and headers
set_target_properties(Configuration-static PROPERTIES
    VERSION ${VERSION}
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)
set_target_properties(Configuration-shared PROPERTIES
    VERSION ${VERSION}
)
install(TARGETS Configuration-shared Configuration-static
  ARCHIVE DESTINATION lib COMPONENT dev   # static lib
  RUNTIME DESTINATION bin COMPONENT libs  # binaries
  LIBRARY DESTINATION lib COMPONENT libs  # shared lib
  FRAMEWORK DESTINATION lib COMPONENT libs # for Mac
  PUBLIC_HEADER DESTINATION include/Configuration COMPONENT dev   # headers
)


# documentation
add_subdirectory(doc)


# For other packages to find this one
set(INCLUDE_INSTALL_DIR include/ CACHE STRING "INCLUDE_INSTALL_DIR")
set(LIB_INSTALL_DIR lib/ CACHE STRING "LIB_INSTALL_DIR")
set(SHARE_INSTALL_DIR share/ CACHE STRING "SHARE_INSTALL_DIR")
include(CMakePackageConfigHelpers)
configure_package_config_file(ConfigurationConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/ConfigurationConfig.cmake
                              INSTALL_DESTINATION ${SHARE_INSTALL_DIR}/Configuration-${VERSION_MAJOR}/cmake
                              PATH_VARS INCLUDE_INSTALL_DIR
                                        LIB_INSTALL_DIR
                                        SHARE_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/ConfigurationConfigVersion.cmake
                                 VERSION ${VERSION}
                                 COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ConfigurationConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ConfigurationConfigVersion.cmake
        DESTINATION ${SHARE_INSTALL_DIR}/Configuration-${VERSION_MAJOR}/cmake )

