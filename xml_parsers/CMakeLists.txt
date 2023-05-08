cmake_minimum_required(VERSION 3.0.0)
project(xml_parsers)

set(CMAKE_CXX_STANDARD 17)

find_package(PugiXML REQUIRED)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(
    ${PROJECT_NAME}
    STATIC
    src/slave_config_file_parser.cpp    
)
target_link_libraries(${PROJECT_NAME} pugixml)
target_include_directories(${PROJECT_NAME} PUBLIC ${PUGI_XML_INCLUDE_DIR} ${INCLUDE_DIR})


