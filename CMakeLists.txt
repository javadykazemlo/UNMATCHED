cmake_minimum_required(VERSION 3.10)

project(UNMATCHED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Local SFML 3 install (MinGW-w64 build). Adjust this path if you move the
# SFML folder; it can also be overridden from the command line with
# -DCMAKE_PREFIX_PATH=<path-to-SFML-install>
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/SFML/install")
endif()

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
find_package(Threads REQUIRED)

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
    Threads::Threads
)

add_custom_command(TARGET a POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:a>/assets)

if(WIN32)
    target_link_libraries(a PRIVATE ws2_32)

    # SFML on Windows is built as shared DLLs by default; copy them next to
    # the executable so it can run without editing PATH.
    if(EXISTS "C:/Libraries/SFML/install/bin")
        add_custom_command(TARGET a POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "C:/Libraries/SFML/install/bin" $<TARGET_FILE_DIR:a>)
    endif()
endif()