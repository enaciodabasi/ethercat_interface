cmake_minimum_required(VERSION 3.0.0)
project(amr_dc)

set(CMAKE_CXX_STANDARD 17)  

set(SOURCES
    amr_controller.cpp
    offset_el7221_9014.hpp
)

set(ethercat_interface_lib /usr/local/lib/libethercat_interface.so)

find_library(${ethercat_interface_lib} REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ethercat_interface_lib} /usr/local/lib/libethercat.so)
target_include_directories(${PROJECT_NAME} PUBLIC ${ethercat_interface_lib_INCLUDE_DIRS})