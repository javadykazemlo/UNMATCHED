cmake_minimum_required(VERSION 3.10)

project(UNMATCHED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
    include/cards
    include/core
    include/entities
)

add_subdirectory(external/ftxui)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(a ${SOURCES})

target_link_libraries(a PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

if(WIN32)
    target_link_libraries(a PRIVATE ws2_32)
endif()