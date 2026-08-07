cmake_minimum_required(VERSION 3.10)

project(UNMATCHED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

include_directories(
    include
    include/cards
    include/core
    include/entities
)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(a ${SOURCES})

target_link_libraries(a PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(TARGET a POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:a>/assets)

if(WIN32)
    target_link_libraries(a PRIVATE ws2_32)
endif()