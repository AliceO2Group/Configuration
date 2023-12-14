# Copyright 2019-2020 CERN and copyright holders of ALICE O2.
# See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
# All rights not expressly granted are reserved.
#
# This software is distributed under the terms of the GNU General Public
# License v3 (GPL Version 3), copied verbatim in the file "COPYING".
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization
# or submit itself to any jurisdiction.
#
# Author Adam Wegrzynek <adam.wegrzynek@cern.ch>

####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)
set(BUILD_SHARED_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

# Set cmake policy by version: https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

# Define project
project(Configuration
  VERSION 2.6.3
  DESCRIPTION "O2 Configuration library"
  LANGUAGES CXX
)

# Documentation dir
add_subdirectory(doc)

# simplified build mode for doc only
if(ONLYDOC)
  return()
endif()

# Add compiler flags for warnings and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# Set fPIC for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMAKE_INSTALL_LIBDIR explicitly to lib (to avoid lib64 on CC7)
set(CMAKE_INSTALL_LIBDIR lib)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE
    STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE
  )
endif()

# Add coverage flags to Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
endif()

####################################
# Dependencies
####################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost 1.56.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(CURL MODULE REQUIRED)
find_package(Git QUIET)
find_package(ppconsul CONFIG)

####################################
# Handle RPATH
####################################
include(GNUInstallDirs)

# https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
  endif()
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
unset(isSystemDir)

####################################
# Library
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

set(SRCS
  src/Backends/Ini/IniBackend.cxx
  src/Backends/String/StringBackend.cxx
  src/Backends/Json/JsonBackend.cxx
  src/Backends/Apricot/ApricotBackend.cxx
  src/ConfigurationInterface.cxx
  src/ConfigurationFactory.cxx
)

# Backends
message(STATUS "Backends")
message(STATUS "  Compiling INI backend")
message(STATUS "  Compiling JSON backend")
message(STATUS "  Compiling STRING backend")

# Create library
add_library(Configuration SHARED ${SRCS} $<$<BOOL:${ppconsul_FOUND}>:src/Backends/Consul/ConsulBackend.cxx>)
target_include_directories(Configuration
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link targets
target_link_libraries(Configuration
  PUBLIC
    Boost::boost
  PRIVATE
    $<$<BOOL:${ppconsul_FOUND}>:ppconsul>
    CURL::libcurl
)

# Handle Ppconsul optional dependency
if(ppconsul_FOUND)
  message(STATUS "  Compiling Ppconsul backend")
endif()

# Handle custom compile definitions
target_compile_definitions(Configuration
  PRIVATE
    $<$<BOOL:${ppconsul_FOUND}>:FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED>
  )

# Use C++17
target_compile_features(Configuration PUBLIC cxx_std_17)


####################################
# Tests
####################################

enable_testing()

set(TEST_SRCS
  test/TestExamples.cxx
  test/TestIni.cxx
  test/TestJson.cxx
  test/TestString.cxx
  test/TestApricot.cxx
)

if(ppconsul_FOUND)
  list(APPEND TEST_SRCS test/TestConsul.cxx)
endif()

foreach (test ${TEST_SRCS})
  get_filename_component(test_name ${test} NAME)
  string(REGEX REPLACE ".cxx" "" test_name ${test_name})

  add_executable(${test_name} ${test})
  target_link_libraries(${test_name}
    PRIVATE
      Configuration Boost::unit_test_framework
  )
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 60)
endforeach()


####################################
# Executables
####################################

add_executable(convert src/CommandLineUtilities/Convert.cxx)
target_link_libraries(convert
  PRIVATE
    Configuration
    Boost::program_options
)
set_target_properties(convert PROPERTIES OUTPUT_NAME "o2-configuration-convert")
add_executable(test-backend src/CommandLineUtilities/TestBackend.cxx)
target_link_libraries(test-backend
  PRIVATE
    Configuration
    Boost::program_options
)
set_target_properties(test-backend PROPERTIES OUTPUT_NAME "o2-configuration-test-backend")
####################################
# Install
####################################

# Install library
install(TARGETS Configuration convert test-backend
  EXPORT ConfigurationTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cmake/ConfigurationConfigVersion.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/Configuration DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Export targets
install(EXPORT ConfigurationTargets
  FILE
    ConfigurationTargets.cmake
  NAMESPACE
    AliceO2::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Configuration
)

# Configure and install Config files
configure_package_config_file(
  cmake/ConfigurationConfig.cmake.in cmake/ConfigurationConfig.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Configuration"
  PATH_VARS CMAKE_INSTALL_PREFIX
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ConfigurationConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ConfigurationConfigVersion.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Configuration
)
