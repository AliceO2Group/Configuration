
########## DEPENDENCIES lookup ############

find_package(Boost 1.42.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
find_package(RapidJSON)
find_package(CURL REQUIRED)
find_package(Protobuf REQUIRED 3.0.0)
find_package(GRPC)

########## General definitions and flags ##########

if (RapidJSON_FOUND)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_ETCD_ENABLED)
endif()

if(PROTOBUF_FOUND AND GRPC_FOUND)
    add_definitions(-DALICEO2_CONFIGURATION_BACKEND_ETCDV3_ENABLED)	  
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

o2_define_bucket(
    NAME
    configuration_bucket_with_etcd3

    DEPENDENCIES
    ${PROTOBUF_LIBRARY}
    grpc 
    grpc++

    SYSTEMINCLUDE_DIRECTORIES
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)

