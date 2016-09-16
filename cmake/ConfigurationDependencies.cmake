
########## DEPENDENCIES lookup ############

find_package(Boost 1.42.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
find_package(RapidJSON)
find_package(CURL REQUIRED)

########## General definitions and flags ##########

if (RapidJSON_FOUND)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_ETCD_ENABLED)
endif()

########## Bucket definitions ############

o2_define_bucket(
    NAME
    configuration_bucket

    DEPENDENCIES
    ${CURL_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}

    SYSTEMINCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
)

o2_define_bucket(
    NAME
    configuration_bucket_with_rapidjson

    DEPENDENCIES
    configuration_bucket

    SYSTEMINCLUDE_DIRECTORIES
    ${RAPIDJSON_INCLUDE_DIRS}
)

#o2_define_bucket(
#    NAME
#    ExampleModule1_bucket
#
#    DEPENDENCIES # library names
#    common_boost_bucket
#    ${Boost_PROGRAM_OPTIONS_LIBRARY}
#
#    INCLUDE_DIRECTORIES
#    ${ROOT_INCLUDE_DIR}
#
#    SYSTEMINCLUDE_DIRECTORIES
#    ${ZMQ_INCLUDE_DIR}
#)

