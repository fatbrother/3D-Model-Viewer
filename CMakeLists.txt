cmake_minimum_required(VERSION 3.22)

project(CG2023_HW)

set(CMAKE_CXX_STANDARD 20)

find_package(FreeGLUT CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

# set source path to src
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

include_directories(${INCLUDE_PATH})

file (GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(CG2023_HW ${SOURCE_FILES})

target_link_libraries(CG2023_HW PRIVATE $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>)
target_link_libraries(CG2023_HW PRIVATE GLEW::GLEW)
target_link_libraries(CG2023_HW PRIVATE glm::glm)
set(cv_libs opencv_ml opencv_dnn opencv_core opencv_flann opencv_imgproc opencv_highgui opencv_imgcodecs)
target_link_libraries(CG2023_HW PRIVATE ${cv_libs})