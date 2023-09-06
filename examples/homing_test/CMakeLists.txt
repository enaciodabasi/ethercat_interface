cmake_minimum_required(VERSION 3.0.0)
project(homing_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

set(SOURCES
    src/homing_test.cpp
)

set(eci_headers /home/naci/Projects/igh_master/include)

set(ethercat_interface_lib /usr/local/lib/libethercat_interface.so)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ethercat_interface_lib} /usr/local/lib/libethercat.so)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${eci_headers})
target_include_directories(${PROJECT_NAME} PUBLIC ${ethercat_interface_lib_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${eci_headers})