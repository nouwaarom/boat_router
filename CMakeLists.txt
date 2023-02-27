cmake_minimum_required(VERSION 3.24)
project(weather_router)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/shapelib)
add_subdirectory(dependencies/ShapefileCpp/lib)

link_directories(${GTK4_LIBRARY_DIRS})

add_definitions(${GTK4_CFLAGS_OTHER})

set(SOURCES src/main.cpp src/shapefilereader.cpp src/chart.cpp src/coordinate.cpp)
set(MESH_SOURCES src/gui/meshes/quad.cpp src/gui/meshes/boat.cpp src/gui/meshes/staticMesh2D.cpp)
set(GUI_SOURCES ${MESH_SOURCES} src/gui/controls.cpp src/gui/shader.cpp src/gui/shaderProgram.cpp src/gui/vertexBufferObject.cpp src/gui/uniform.cpp src/gui/viewport.cpp src/chart.cpp inc/chart.h inc/gui/controls.h src/gui/controls.cpp inc/application.h src/application.cpp)

add_executable(weather_router ${SOURCES} ${GUI_SOURCES})
target_include_directories(weather_router PRIVATE inc inc/gui inc/gui/meshes ${GTK4_INCLUDE_DIRS})
target_link_libraries(weather_router PRIVATE ShapefileCpp ${GTK4_LIBRARIES} glm::glm glad OpenGL::OpenGL stdc++)
