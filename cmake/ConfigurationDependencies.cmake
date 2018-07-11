find_package(Boost 1.56.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
find_package(CURL REQUIRED)
find_package(Ppconsul)
find_package(RapidJSON)

set(extra_deps "")
set(extra_deps_include "")

if(RAPIDJSON_FOUND)
    message(STATUS "RapidJSON found: include ${RAPIDJSON_INCLUDE_DIRS}")
    add_definitions(-DFLP_CONFIGURATION_BACKEND_FILE_JSON_ENABLED)
    list(APPEND extra_deps_include ${RAPIDJSON_INCLUDE_DIRS})
else()
    message(STATUS "RapidJSON not found")
endif()

if(PPCONSUL_FOUND)
    message(STATUS "PpConsul found : ${PPCONSUL_LIBRARIES}; include ${PPCONSUL_INCLUDE_DIRS}")
    add_definitions(-DFLP_CONFIGURATION_BACKEND_CONSUL_ENABLED)
    list(APPEND extra_deps ${PPCONSUL_LIBRARIES})
    list(APPEND extra_deps_include ${PPCONSUL_INCLUDE_DIRS})
else()
    message(STATUS "PpConsul not found")
endif()


o2_define_bucket(
    NAME
    configuration_bucket

    DEPENDENCIES
    ${CURL_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${extra_deps}

    SYSTEMINCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${extra_deps_include}
)
