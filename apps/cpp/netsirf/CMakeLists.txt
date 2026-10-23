cmake_minimum_required(VERSION 3.10)
project(web_browser)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.1)

add_executable(web_browser browser.cpp)

target_include_directories(web_browser PRIVATE ${GTK3_INCLUDE_DIRS} ${WEBKIT_INCLUDE_DIRS})
target_link_libraries(web_browser PRIVATE ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES})