cmake_minimum_required(VERSION 3.0.0)
project(ethercat_config_gui)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core REQUIRED)
find_package(Threads REQUIRED)

set(include_dir include/${PROJECT_NAME})

set(
    SOURCES
    src/main.cpp
    src/main_window.cpp
    ${include_dir}/main_window.hpp
    src/central_widget.cpp
    ${include_dir}/central_widget.hpp
    src/ec_line_display.cpp
    ${include_dir}/ec_line_display.hpp
    src/slave_info_widget.cpp
    ${include_dir}/slave_info_widget.hpp
    ${include_dir}/defs.hpp

)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
