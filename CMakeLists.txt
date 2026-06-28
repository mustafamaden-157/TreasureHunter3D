cmake_minimum_required(VERSION 3.15)
project(TreasureHunter3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bilgisayar Grafikleri dersi için OpenGL tabanlı hazine toplama oyunu.
# Bu dosya, projenin CMake ile derlenmesi için temel yapılandırmayı içerir.

add_executable(TreasureHunter3D
    src/main.cpp
)

# NOT:
# GLFW, GLAD ve GLM kütüphanelerinin yolları bilgisayardaki kurulum şekline göre
# ayrıca eklenmelidir.
#
# Örnek:
# target_include_directories(TreasureHunter3D PRIVATE external/glad/include external/glfw/include external/glm)
# target_link_libraries(TreasureHunter3D PRIVATE glfw glad)
