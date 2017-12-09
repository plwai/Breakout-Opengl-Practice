cmake_minimum_required (VERSION 3.7.0 FATAL_ERROR)
project (Breakout)

find_package(OpenGL REQUIRED)
find_package(GLFW3 3.2 REQUIRED)
find_package(GLM 0.9.9 REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# version
set (ROEChip8_VERSION_MAJOR 0)
set (ROEChip8_VERSION_MINOR 1)

# path
set (SOURCE_PATH "${PROJECT_SOURCE_DIR}")

# config file
configure_file ( 
	"${PROJECT_SOURCE_DIR}/src/BreakoutConfig.h.in" 
	"${PROJECT_BINARY_DIR}/BreakoutConfig.h"
)

set ( source_files
	src/main.cc
	src/glad/glad.c
)

set ( header_files

)

set ( shader_files

)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(Breakout ${source_files} ${header_files} ${shader_files})

source_group("Shader" FILES ${shader_files})

target_include_directories(Breakout PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Breakout PUBLIC "${PROJECT_SOURCE_DIR}/modules/glad/include")
target_include_directories(Breakout PUBLIC "${PROJECT_SOURCE_DIR}/modules/stb")
target_include_directories(Breakout PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Breakout PUBLIC ${GLM_INCLUDE_DIR})
target_include_directories(Breakout PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(Breakout ${OPENGL_gl_LIBRARY})
target_link_libraries(Breakout glfw)
target_link_libraries(Breakout glm)

install (TARGETS Breakout DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/BreakoutConfig.h" DESTINATION include)