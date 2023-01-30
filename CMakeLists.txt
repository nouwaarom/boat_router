cmake_minimum_required(VERSION 3.24)
project(weather_router)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(weather_router main.cpp)
target_link_libraries(weather_router PRIVATE ${GTK4_LIBRARIES} OpenGL::OpenGL)
