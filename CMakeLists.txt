cmake_minimum_required(VERSION 3.0.0)
project(ethercat_interface)

find_package(yaml-cpp REQUIRED) 

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/controller.cpp
    src/slave.cpp
    src/utilities.cpp
)

set(
    etherlab_include
    /home/${USER}/ethercat/include
)

set(
    etherlab_lib
    /usr/local/lib/libethercat.so
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SOURCES}
)
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${etherlab_include}
    ${YAML_CPP_INCLUDE_DIRS}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${etherlab_lib}
    ${YAML_CPP_LIBRARIES}
)

#set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(${PROJECT_NAME}_PUBLIC_HEADERS
    include/slave.hpp
    include/offset.hpp
    include/utilities.hpp
    include/controller.hpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    PUBLIC_HEADER 
    "${${PROJECT_NAME}_PUBLIC_HEADERS}"
    
)

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ethercat_interface
)