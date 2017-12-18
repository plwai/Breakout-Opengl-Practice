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
set (SHADER_PATH "${PROJECT_SOURCE_DIR}/src/shader")
set (LEVEL_PATH "${PROJECT_SOURCE_DIR}/src/levels")

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
	${BREAKOUT_SOURCE_PATH}/spriteRenderer.cpp
	${BREAKOUT_SOURCE_PATH}/gameObject.cpp
	${BREAKOUT_SOURCE_PATH}/gameLevel.cpp
	${BREAKOUT_SOURCE_PATH}/ballObject.cpp
	${BREAKOUT_SOURCE_PATH}/particleGenerator.cpp
	${BREAKOUT_SOURCE_PATH}/postProcessor.cpp
)

set ( header_files
	${BREAKOUT_SOURCE_PATH}/gameSetting.h
	${BREAKOUT_SOURCE_PATH}/shader.h
	${BREAKOUT_SOURCE_PATH}/texture.h
	${BREAKOUT_SOURCE_PATH}/resourceManager.h
	${BREAKOUT_SOURCE_PATH}/spriteRenderer.h
	${BREAKOUT_SOURCE_PATH}/gameObject.h
	${BREAKOUT_SOURCE_PATH}/ballObject.h
	${BREAKOUT_SOURCE_PATH}/particleGenerator.h
	${BREAKOUT_SOURCE_PATH}/postProcessor.h
)

set ( module_source_files
	src/glad/glad.c
)

set ( shader_files
	${SHADER_PATH}/sprite.vs
	${SHADER_PATH}/particle.vs
	${SHADER_PATH}/postProcess.vs

	${SHADER_PATH}/sprite.frag
	${SHADER_PATH}/particle.frag
	${SHADER_PATH}/postProcess.frag
)

set ( level_files
	${LEVEL_PATH}/level_1.lvl
	${LEVEL_PATH}/level_2.lvl
	${LEVEL_PATH}/level_3.lvl
	${LEVEL_PATH}/level_4.lvl
)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(Breakout ${source_files} ${header_files} ${shader_files} ${module_source_files} ${level_files})

source_group("Shader Files" FILES ${shader_files})
source_group("Module Source" FILES ${module_source_files})
source_group("Level Files" FILES ${level_files})

target_include_directories(Breakout PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Breakout PUBLIC "${PROJECT_SOURCE_DIR}/modules/glad/include")
target_include_directories(Breakout PUBLIC "${PROJECT_SOURCE_DIR}/modules/stb")
target_include_directories(Breakout PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Breakout PUBLIC ${GLM_INCLUDE_DIR})
target_link_libraries(Breakout ${OPENGL_gl_LIBRARY})
target_link_libraries(Breakout glfw)
target_link_libraries(Breakout glm)

install (TARGETS Breakout DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/BreakoutConfig.h" DESTINATION include)