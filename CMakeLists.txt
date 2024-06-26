cmake_minimum_required(VERSION 3.0.0)
project(ethercat_interface VERSION 2.0.0 LANGUAGES C CXX)

find_package(yaml-cpp REQUIRED) 

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    
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
    ${Boost_INCLUDE_DIR}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${etherlab_lib}
    ${YAML_CPP_LIBRARIES}
    ${Boost_LIBRARIES}
)

#set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(${PROJECT_NAME}_PUBLIC_HEADERS
    
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

#add_executable(
#    offset_generator
#    generator/offset_generator.cpp
#)
#target_link_libraries(offset_generator ${YAML_CPP_LIBRARIES})
#target_include_directories(offset_generator PUBLIC ${YAML_CPP_INCLUDE_DIRS})

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_parsers
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_parsers/build
)

#add_subdirectory(
#    ${CMAKE_CURRENT_SOURCE_DIR}/ethercat_config_gui
#    ${CMAKE_CURRENT_SOURCE_DIR}/ethercat_config_gui/build
#    )