cmake_minimum_required(VERSION 3.5)
project(moongl VERSION 1.0 LANGUAGES C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(Lua CONFIG)
find_package(GLEW CONFIG)
find_package(glfw3)

file(GLOB_RECURSE  SOURCES_C "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_C})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_HEADER_PUB})

add_library(moongl 
	${SOURCES_HEADER_PUB}

	src/draw.c
	src/get.c
	src/init.c
	src/object.c
	src/query.c
	src/sync.c
	src/utils.c
	src/bitfield.c
	src/enums.c
	src/getintformat.c
	src/perfragment.c
	src/raster.c
	src/teximage.c
	src/vertex_array.c
	src/getstring.c
	src/main.c
	src/pipeline.c
	src/renderbuffer.c
	src/texture.c
	src/whole_framebuffer.c
	src/buffer.c
	src/framebuffer.c
	src/getuniform.c
	src/pixel.c
	src/sampler.c
	src/transform.c
	src/capabilities.c
	src/func.c
	src/getvertex.c
	src/proginterface.c
	src/shader.c
	src/debug.c
	src/hint.c
	src/nongl.c
	src/program.c
	src/subroutine.c
	src/uniform.c

)

target_include_directories(moongl
    PUBLIC
        $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(moongl PUBLIC 
	Lua::Lua
	GLEW
	glfw
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(moongl moongl "" lib/cmake/moongl)
