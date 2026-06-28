cmake_minimum_required(VERSION 3.15)
project(TreasureHunter3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)

add_executable(TreasureHunter3D
    src/main.cpp
)

target_link_libraries(TreasureHunter3D
    glfw
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)
