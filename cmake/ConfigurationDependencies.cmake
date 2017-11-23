
########## DEPENDENCIES lookup ############

find_package(Boost 1.56.0 COMPONENTS unit_test_framework program_options REQUIRED)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
find_package(RapidJSON)
find_package(CURL REQUIRED)
find_package(Protobuf 3.0.0)
find_package(GRPC)
find_package(PpConsul)
find_package(Common REQUIRED)
find_package(MySQL)

if(MYSQL_FOUND)
    message(STATUS "MySQL found : ${MYSQL_LIBRARIES}; include ${MYSQL_INCLUDE_DIRS}")
    add_definitions(-DFLP_CONFIGURATION_BACKEND_MYSQL_ENABLED)
else()
    message(WARNING "MySQL not found")
endif()

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

# Message as Protobuf is silent
if(PROTOBUF_FOUND)
    message(STATUS "ProtoBuf found : ${PROTOBUF_LIBRARIES}; include ${PROTOBUF_INCLUDE_DIRS}")
else()
    message(STATUS "ProtoBuf not found")
endif()

# Message as GRPC is silent when it's not found
if(GRPC_FOUND)
    message(STATUS "gRPC found : ${GRPC_LIBRARIES}; include ${GRPC_INCLUDE_DIR}")
else()
    message(STATUS "gRPC not found")
endif()


########## General definitions and flags ##########

if (RAPIDJSON_FOUND AND PROTOBUF_FOUND AND GRPC_FOUND AND PPCONSUL_FOUND)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_ETCD_ENABLED)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_FILE_JSON_ENABLED)
    add_definitions(-DFLP_CONFIGURATION_BACKEND_ETCDV3_ENABLED)
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
    ${Common_LIBRARIES}
    ${MYSQL_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${Common_INCLUDE_DIRS}
    ${MYSQL_INCLUDE_DIRS}
)

# This bucket does not inherit from configuration_bucket because we want to enforce a certain order of includes.
o2_define_bucket(
    NAME
    configuration_bucket_with_rapidjson_etcd3

    DEPENDENCIES
    ${PROTOBUF_LIBRARY}
    ${GRPC_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${CURL_LIBRARIES}
    ${PPCONSUL_LIBRARIES}
    ${Common_LIBRARIES}
    ${MYSQL_LIBRARIES}

    SYSTEMINCLUDE_DIRECTORIES
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIRS}
    ${PPCONSUL_INCLUDE_DIR}
    ${Common_INCLUDE_DIRS}
    ${MYSQL_INCLUDE_DIRS}
)

