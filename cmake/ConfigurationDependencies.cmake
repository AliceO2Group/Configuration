
########## DEPENDENCIES lookup ############

find_package(Boost 1.56.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
find_package(CURL REQUIRED)
find_package(Common REQUIRED)
find_package(PpConsul)
find_package(RapidJSON)

# Message as RapidJSON is silent when it's not found
if(RAPIDJSON_FOUND)
    message(STATUS "RapidJSON found: include ${RAPIDJSON_INCLUDE_DIRS}")
else()
    message(STATUS "RapidJSON not found")
endif()

# Message as PpConsul is silent
if(PPCONSUL_FOUND)
    message(STATUS "PpConsul found : ${PPCONSUL_LIBRARIES}; include ${PPCONSUL_INCLUDE_DIRS}")
else()
    message(STATUS "PpConsul not found")
endif()


########## General definitions and flags ##########

if (RAPIDJSON_FOUND AND PPCONSUL_FOUND)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_FILE_JSON_ENABLED)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_CONSUL_ENABLED)
endif()

#if (MYSQL_NDB_FOUND)
#    add_definitions(-DFLP_CONFIGURATION_BACKEND_NDB_ENABLED)
#endif()


########## Bucket definitions ############

o2_define_bucket(
    NAME
    configuration_bucket

    DEPENDENCIES
    ${CURL_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${MYSQL_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${MYSQL_INCLUDE_DIRS}
)


# This bucket does not inherit from configuration_bucket because we want to enforce a certain order of includes.
o2_define_bucket(
    NAME
    configuration_bucket_with_rapidjson_with_consul

    DEPENDENCIES
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${CURL_LIBRARIES}
    ${PPCONSUL_LIBRARIES}
    ${Common_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${CURL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIRS}
    ${PPCONSUL_INCLUDE_DIR}
    ${Common_INCLUDE_DIRS}
)

o2_define_bucket(
    NAME
    configuration_app_bucket

    DEPENDENCIES
    configuration_bucket
    ${Common_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${Common_INCLUDE_DIRS}
  )

o2_define_bucket(
    NAME
    configuration_app_bucket_with_rapidjson_with_consul

    DEPENDENCIES
    configuration_bucket_with_rapidjson_with_consul
    ${Common_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${Common_INCLUDE_DIRS}
  )
