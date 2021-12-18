cmake_minimum_required(VERSION 3.5)
project(ejoy2d VERSION 1.0)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(Lua CONFIG)
find_package(GLEW CONFIG)

file(GLOB_RECURSE  SOURCES_C "lib/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "lib/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/lib PREFIX "Source Files"  FILES ${SOURCES_C})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/lib PREFIX "Source Files"  FILES ${SOURCES_HEADER_PUB})

add_library(ejoy2d 
	${SOURCES_HEADER_PUB}

	lib/render/render.c 
	lib/render/carray.c 
	lib/render/log.c

	lib/shader.c 
	lib/lshader.c 
	lib/ejoy2dgame.c 
	lib/fault.c 
	lib/screen.c 
	lib/texture.c 
	lib/ppm.c 
	lib/spritepack.c 
	lib/sprite.c 
	lib/lsprite.c 
	lib/matrix.c 
	lib/lmatrix.c 
	lib/dfont.c 
	lib/label.c 
	lib/particle.c 
	lib/lparticle.c 
	lib/scissor.c 
	lib/renderbuffer.c 
	lib/lrenderbuffer.c 
	lib/lgeometry.c
)

target_include_directories(ejoy2d
    PUBLIC
        $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/render>
)

target_link_libraries(ejoy2d PUBLIC 
	Lua::Lua
	GLEW
)

# if(WIN32)
# target_compile_definitions(ejoy2d 
#   PRIVATE
# 	-DNOMINMAX
# 	EJOY2D_OS=WINDOWS
# 	_DEBUG
# 	_WINDOWS
# )
# endif(WIN32)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(ejoy2d ejoy2d "" lib/cmake/ejoy2d)
