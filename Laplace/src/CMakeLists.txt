#From root project directory (highest level):
#cmake -B build -S ./src/ -DCMAKE_TOOLCHAIN_FILE=/home/roboevt/dev/c++/OpenGLRayTracer/OpenGLRayTracer/vcpkg/scripts/buildsystems/vcpkg.cmake
#cmake --build ./build/ --parallel 12

cmake_minimum_required( VERSION 3.12 )

project( OpenGLRayTracer)

include(/home/roboevt/dev/c++/OpenGLRayTracer/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package( OpenGL REQUIRED )
find_package( glfw3 REQUIRED )
find_package( GLEW REQUIRED )
find_package( glm CONFIG REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS} )

file(GLOB SOURCES *.cpp)

add_compile_options(-Wall -std=c++20 -O3 -march=native -fno-omit-frame-pointer)

add_executable( OpenGLRayTracer ${SOURCES} )

target_link_libraries( OpenGLRayTracer ${OPENGL_LIBRARIES} glfw GLEW::GLEW glm::glm)