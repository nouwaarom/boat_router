cmake_minimum_required(VERSION 3.24)
project(weather_router)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_subdirectory(dependencies/shapelib)
add_subdirectory(dependencies/ShapefileCpp/lib)

link_directories(${GTK4_LIBRARY_DIRS})

add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(weather_router src/main.cpp src/shapefilereader.cpp)
target_include_directories(weather_router PRIVATE inc ${GTK4_INCLUDE_DIRS})
target_link_libraries(weather_router PRIVATE ShapefileCpp ${GTK4_LIBRARIES} GLEW::GLEW OpenGL::OpenGL stdc++)
