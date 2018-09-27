####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)

# Set cmake policy by version: https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

# Define project
project(Configuration
  VERSION 1.5.2
  DESCRIPTION "O2 Configuration library"
  LANGUAGES CXX
)

# Documentation dir
add_subdirectory(doc)

# Add compiler flags for warnings and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# Set fPIC for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
find_package(Git QUIET)
find_package(Ppconsul MODULE)
find_package(CURL REQUIRED MODULE)


####################################
# Library
####################################

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

set(SRCS
  src/Backends/Ini/IniBackend.cxx
  src/Backends/Json/JsonBackend.cxx
  src/ConfigurationInterface.cxx
  src/ConfigurationFactory.cxx
)

# Backends
message(STATUS "Backends")
message(STATUS "  Compiling INI backend")
message(STATUS "  Compiling JSON backend")

# Create library
add_library(Configuration SHARED ${SRCS} $<$<BOOL:${Ppconsul_FOUND}>:src/Backends/Consul/ConsulBackend.cxx>)
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
    CURL::CURL
    $<$<BOOL:${Ppconsul_FOUND}>:Ppconsul::Ppconsul>
)

# Handle Ppconsul optional dependency
if(Ppconsul_FOUND)
  message(STATUS "  Compiling Ppconsul backend")
endif()

# Handle custom compile definitions
target_compile_definitions(Configuration
  PRIVATE
    $<$<BOOL:${Ppconsul_FOUND}>:FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED>
  )

# Use C++14
target_compile_features(Configuration PUBLIC cxx_std_14)


####################################
# Tests
####################################

enable_testing()

set(TEST_SRCS
  test/TestExamples.cxx
  test/TestIni.cxx
  test/TestJson.cxx
)

if(Ppconsul_FOUND)
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
# Install
####################################

include(GNUInstallDirs)

# Build targets with install rpath on Mac to dramatically speed up installation
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

# Install library
install(TARGETS Configuration
  EXPORT ConfigurationTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
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
