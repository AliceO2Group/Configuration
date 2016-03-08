# @author  Barthélémy von Haller
set(LIB_HEADERS
  "${CMAKE_CURRENT_BINARY_DIR}/../include/Configuration/Version.h" # Generated header
  ../include/Configuration/Configuration.h # Normal header
  ../include/Configuration/ConfigurationInterface.h # Normal header
  ../include/Configuration/EtcdConfiguration.h # Normal header
  ../include/Configuration/ConfigurationFactory.h
  ../include/Configuration/FileConfiguration.h
)

set(LIB_SOURCES
  Configuration.cxx
  ConfigurationInterface.cxx
  ConfigurationFactory.cxx
  EtcdConfiguration.cxx
  FileConfiguration.cxx
)

# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME_SHARED Configuration-shared)
set(LIBRARY_NAME_STATIC Configuration-static)

# Define the library
add_library(${LIBRARY_NAME_SHARED} SHARED ${LIB_SOURCES})
add_library(${LIBRARY_NAME_STATIC} STATIC ${LIB_SOURCES})

# Link
target_link_libraries(${LIBRARY_NAME_SHARED} ${CURL_LIBRARIES})
target_link_libraries(${LIBRARY_NAME_STATIC} ${CURL_LIBRARIES})

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

