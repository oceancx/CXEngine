cmake_minimum_required (VERSION 3.10)
project(NanoVG  VERSION 1.0.0 LANGUAGES C CXX)


if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files\\include"  FILES ${SOURCES_HEADER_PUB})

add_library(nanovg
    ${SOURCES_HEADER_PUB}
    src/nanovg.c
)
set(NANOVG_C_SOURCE 
    src/nanovg.c
)
set_source_files_properties(${NANOVG_SOURCE} PROPERTIES LANGUAGE C)

target_include_directories(nanovg
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


find_package(GLEW CONFIG)
find_package(glfw3)
add_executable(nanovg_demo
    example/demo.c
    example/perf.c
    example/example_gl3.c
)
target_include_directories(nanovg_demo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example
)
target_link_libraries(nanovg_demo
PRIVATE
    nanovg
    glfw
    GLEW
)
target_compile_definitions(nanovg_demo
  PRIVATE
    -DGLEW_STATIC
    -DNANOVG_GLEW
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(nanovg nanovg "" lib/cmake/nanovg)
