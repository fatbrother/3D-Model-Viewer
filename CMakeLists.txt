cmake_minimum_required(VERSION 3.22)

project(CG2023_HW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)

find_package(FreeGLUT CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

# set source path to src
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

include_directories(${INCLUDE_PATH})

file (GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(CG2023_HW ${SOURCE_FILES})

target_link_libraries(CG2023_HW PRIVATE $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>)
target_link_libraries(CG2023_HW PRIVATE GLEW::GLEW)
target_link_libraries(CG2023_HW PRIVATE glm::glm)
