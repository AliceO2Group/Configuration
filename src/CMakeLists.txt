# @author  Barthélémy von Haller
set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Configuration/Version.h" # Generated header
  ../include/Configuration/Configuration.h # Normal header
  ../include/Configuration/ConfigurationInterface.h # Normal header
  ../include/Configuration/ConfigurationFactory.h # Normal header
)

set(LIB_SOURCES
  Configuration.cxx
  ConfigurationInterface.cxx
  ConfigurationFactory.cxx
  FileConfiguration.cxx
)

set(CONF_LINK_LIBS
  Common
  InfoLogger-shared
  ${CURL_LIBRARIES}
  pthread
)

# Add ETCD backend interface if the RapidJSON dependency was found
if(RapidJSON_FOUND)
  add_definitions(-DALICEO2_CONFIGURATION_BACKEND_ETCD_ENABLED)
  include_directories(${RAPIDJSON_INCLUDE_DIRS})
  list(APPEND LIB_SOURCES EtcdConfiguration.cxx)
  list(APPEND LIB_HEADERS ../include/Configuration/EtcdConfiguration.h) # Normal header
endif()

# Add ETCD v3 backend if the protocol buffer dependency was found 
if(PROTOBUF_FOUND)
  add_definitions(-DALICEO2_CONFIGURATION_BACKEND_ETCDV3_ENABLED)
  include_directories(${PROTOBUF_INCLUDE_DIRS})
  list(APPEND CONF_LINK_LIBS ${PROTOBUF_LIBRARY})
  list(APPEND CONF_LINK_LIBS grpc grpc++)
  list(APPEND LIB_SOURCES 
    EtcdV3/proto/auth.grpc.pb.cc
    EtcdV3/proto/auth.pb.cc
    EtcdV3/proto/kv.grpc.pb.cc
    EtcdV3/proto/kv.pb.cc
    EtcdV3/proto/rpc.grpc.pb.cc
    EtcdV3/proto/rpc.pb.cc
  )
  # Note: these should probably be replaced by a google api lib installed in the system.
  list(APPEND LIB_SOURCES 
    EtcdV3/proto/googleapis/google/api/annotations.grpc.pb.cc
    EtcdV3/proto/googleapis/google/api/annotations.pb.cc
    EtcdV3/proto/googleapis/google/api/http.grpc.pb.cc
    EtcdV3/proto/googleapis/google/api/http.pb.cc
  )
endif()

# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME_SHARED Configuration-shared)
set(LIBRARY_NAME_STATIC Configuration-static)

# Define the library
add_library(${LIBRARY_NAME_SHARED} SHARED ${LIB_SOURCES})
add_library(${LIBRARY_NAME_STATIC} STATIC ${LIB_SOURCES})

# Link
target_link_libraries(${LIBRARY_NAME_SHARED} ${CONF_LINK_LIBS})
target_link_libraries(${LIBRARY_NAME_STATIC} ${CONF_LINK_LIBS})

# Example program
set(ETCD_EXAMPLE_NAME EtcdExample)
add_executable(${ETCD_EXAMPLE_NAME} EtcdExample.cxx)
target_link_libraries(${ETCD_EXAMPLE_NAME} ${LIBRARY_NAME_SHARED})


# Set the build version. It will be used in the name of the lib, with corresponding
# symlinks created. SOVERSION could also be specified for api version. 
set_target_properties(${LIBRARY_NAME_SHARED} PROPERTIES
  VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
  FRAMEWORK FALSE
  PUBLIC_HEADER "${LIB_HEADERS}" # specify the public headers
)
set_target_properties(${LIBRARY_NAME_STATIC} PROPERTIES
        VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
        FRAMEWORK FALSE
        PUBLIC_HEADER "${LIB_HEADERS}" # specify the public headers
        )

# Says how and where to install software
install(TARGETS ${LIBRARY_NAME_SHARED} ${LIBRARY_NAME_STATIC}
  EXPORT ${PROJECT_NAME}Targets            # for downstream dependencies
  ARCHIVE DESTINATION lib COMPONENT libs   # static lib
  RUNTIME DESTINATION bin COMPONENT libs   # binaries
  LIBRARY DESTINATION lib COMPONENT libs   # shared lib
  FRAMEWORK DESTINATION bin COMPONENT libs # for mac
  PUBLIC_HEADER DESTINATION include/${PROJECT_NAME} COMPONENT devel   # headers for mac (note the different component -> different package)
  INCLUDES DESTINATION include             # headers
)

