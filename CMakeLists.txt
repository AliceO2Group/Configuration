####################################
# General project definition
####################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0 FATAL_ERROR)

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.

FOREACH (p
        CMP0025 # CMake 3.0
        CMP0042 # CMake 3.0
        )
    IF (POLICY ${p})
        cmake_policy(SET ${p} NEW)
    ENDIF ()
endforeach ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir

project(Configuration)

# Build targets with install rpath on Mac to dramatically speed up installation
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
  endif()
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
unset(isSystemDir)

# Load some basic macros which are needed later on
include(O2Utils)
include(ConfigurationDependencies)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"
            CACHE
            STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# C++14
IF (CMAKE_VERSION VERSION_LESS 3.1)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++14 COMPILER_SUPPORTS_CXX14)
    if (COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else ()
        message(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
    endif ()
ELSE ()
    set(CMAKE_CXX_STANDARD 14)  # proper way in CMake >= 3.1
ENDIF ()

# Add compiler flags for warnings and (more importantly) fPIC and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -fPIC")


####################################
# Module, library and executable definition
####################################

set(MODULE_NAME "Configuration")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/Backends/Ini/IniBackend.cxx
        src/Backends/Json/JsonBackend.cxx
        src/ConfigurationInterface.cxx
        src/ConfigurationFactory.cxx
        )

# Produce the final Version.h using template Version.h.in and substituting variables.
# We don't want to polute our source tree with it, thus putting it in binary tree.
configure_file("include/${MODULE_NAME}/Version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/${MODULE_NAME}/Version.h"
        @ONLY)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/include
)

set(HEADERS # needed for the dictionary generation
        "${CMAKE_CURRENT_BINARY_DIR}/include/${MODULE_NAME}/Version.h" # Generated header
        include/${MODULE_NAME}/ConfigurationInterface.h # Normal header
        include/${MODULE_NAME}/ConfigurationFactory.h # Normal header
        include/${MODULE_NAME}/Tree.h # Normal header
        include/${MODULE_NAME}/Visitor.h # Normal header
        include/${MODULE_NAME}/VisitorImplementation.h # Normal header
        )

set(LIBRARY_NAME ${MODULE_NAME})

if(PPCONSUL_FOUND)
    list(APPEND SRCS src/Backends/Consul/ConsulBackend.cxx)
endif()

set(BUCKET_NAME configuration_bucket)

O2_GENERATE_LIBRARY()

set(LIBRARY_NAME ${MODULE_NAME})

message(STATUS LIBRARY_NAME: ${LIBRARY_NAME})
message(STATUS BUCKET_NAME: ${BUCKET_NAME})

enable_testing()

set(TEST_SRCS
        test/TestExamples.cxx
        test/TestBackends.cxx
        )

O2_GENERATE_TESTS(
        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS ${TEST_SRCS}
)

add_subdirectory(doc)
