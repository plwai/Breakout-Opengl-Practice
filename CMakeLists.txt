cmake_minimum_required (VERSION 3.7.0 FATAL_ERROR)
project (Breakout)

find_package(OpenGL REQUIRED)
find_package(GLFW3 3.2 REQUIRED)
find_package(GLM 0.9.9 REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# version
set (Breakout_VERSION_MAJOR 0)
set (Breakout_VERSION_MINOR 1)

# path
set (ROOT_PATH "${PROJECT_SOURCE_DIR}")
set (BREAKOUT_SOURCE_PATH "${PROJECT_SOURCE_DIR}/src/breakout")

# config file
configure_file ( 
	"${PROJECT_SOURCE_DIR}/src/BreakoutConfig.h.in" 
	"${PROJECT_BINARY_DIR}/BreakoutConfig.h"
)

set ( source_files
	src/main.cc

	${BREAKOUT_SOURCE_PATH}/gameSetting.cpp
	${BREAKOUT_SOURCE_PATH}/shader.cpp
	${BREAKOUT_SOURCE_PATH}/texture.cpp
	${BREAKOUT_SOURCE_PATH}/resourceManager.cpp
)

set ( header_files
	${BREAKOUT_SOURCE_PATH}/gameSetting.h
	${BREAKOUT_SOURCE_PATH}/shader.h
	${BREAKOUT_SOURCE_PATH}/texture.h
	${BREAKOUT_SOURCE_PATH}/resourceManager.h
)

set ( module_source_files
	src/glad/glad.c
)

set ( shader_files

)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(Breakout ${source_files} ${header_files} ${shader_files} ${module_source_files})

source_group("Shader" FILES ${shader_files})
source_group("Module Source" FILES ${module_source_files})

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