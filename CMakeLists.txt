cmake_minimum_required(VERSION 3.22)

project(CG2023_HW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

find_package(FreeGLUT CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

# set source path to src
set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)

include_directories(${SOURCE_PATH})

file (GLOB_RECURSE SOURCE_FILES ${SOURCE_PATH}/*.cpp ${SOURCE_PATH}/*.h)

# add executable
add_executable(CG2023_HW ${SOURCE_FILES})

target_link_libraries(CG2023_HW PRIVATE $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>)
target_link_libraries(CG2023_HW PRIVATE GLEW::GLEW)
target_link_libraries(CG2023_HW PRIVATE glm::glm)