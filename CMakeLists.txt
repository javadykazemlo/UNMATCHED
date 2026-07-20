cmake_minimum_required(VERSION 3.10)

project(UNMATCHED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# مسیرهای include
include_directories(
    include
    include/cards
    include/core
    include/entities
)

# FTXUI
add_subdirectory(external/ftxui)

# جمع‌آوری فایل‌های منبع
file(GLOB_RECURSE SOURCES "src/*.cpp")

# ساخت executable با اسم "a"
add_executable(a ${SOURCES})

# لینک کردن کتابخانه‌ها
target_link_libraries(a
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# برای ویندوز
if (WIN32)
    target_link_libraries(a
        ws2_32
    )
endif()