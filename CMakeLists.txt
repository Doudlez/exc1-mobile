cmake_minimum_required(VERSION 2.8)
project(pexc1_animated_mobile)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES main.cpp source/List.c source/LoadShader.c source/OBJParser.c source/StringExtra.c source/Mesh.cpp source/Mesh.h source/Instance.cpp source/Instance.h)
add_executable(pexc1_animated_mobile ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
        glm
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS})
target_link_libraries(pexc1_animated_mobile ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
