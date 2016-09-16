####################################
# General project definition
####################################

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

# C++11
IF (CMAKE_VERSION VERSION_LESS 3.1)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 COMPILER_SUPPORTS_CXX11)
    if (COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else ()
        message(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif ()
ELSE ()
    set(CMAKE_CXX_STANDARD 11)  # proper way in CMake >= 3.1
ENDIF ()

# Add compiler flags for warnings and (more importantly) fPIC and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -fPIC")


####################################
# Module, library and executable definition
####################################

set(MODULE_NAME "Configuration")

O2_SETUP(NAME ${MODULE_NAME})

set(SRCS
        src/Configuration.cxx
        src/ConfigurationInterface.cxx
        src/ConfigurationFactory.cxx
        src/FileConfiguration.cxx
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
        include/${MODULE_NAME}/Configuration.h # Normal header
        include/${MODULE_NAME}/ConfigurationInterface.h # Normal header
        include/${MODULE_NAME}/ConfigurationFactory.h # Normal header
        include/${MODULE_NAME}/FileConfiguration.h # Normal header
        )

set(LIBRARY_NAME ${MODULE_NAME})

if (RapidJSON_FOUND)
    set(BUCKET_NAME_CONFIG configuration_bucket_with_rapidjson)
    list(APPEND SRCS
            EtcdConfiguration.cxx
            )
    list(APPEND HEADERS
            ../include/Configuration/EtcdConfiguration.h
            )
else ()
    set(BUCKET_NAME_CONFIG configuration_bucket)
endif ()
set(BUCKET_NAME ${BUCKET_NAME_CONFIG})

O2_GENERATE_LIBRARY()

# todo we repeat ourselves because the above macro dares deleting the variables we pass to it.
set(LIBRARY_NAME ${MODULE_NAME})
set(BUCKET_NAME ${BUCKET_NAME_CONFIG})

# Example program
O2_GENERATE_EXECUTABLE(
        EXE_NAME EtcdExample
        SOURCES src/EtcdExample.cxx
        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
        BUCKET_NAME ${BUCKET_NAME}
)

# Script
O2_GENERATE_EXECUTABLE(
        EXE_NAME DatasetGenerator
        SOURCES scripts/DatasetGenerator.cxx
        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
        BUCKET_NAME ${BUCKET_NAME}
)

enable_testing()

set(TEST_SRCS
        test/testConfiguration.cxx
        )

O2_GENERATE_TESTS(
        MODULE_LIBRARY_NAME ${LIBRARY_NAME}
        BUCKET_NAME ${BUCKET_NAME}
        TEST_SRCS ${TEST_SRCS}
)
