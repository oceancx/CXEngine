cmake_minimum_required (VERSION 3.10)
project(miniaudio  VERSION 1.0.0 LANGUAGES C CXX)


if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files\\include"  FILES ${SOURCES_HEADER_PUB})

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


include(cmake/HelperMacroFunction.cmake)
export_library_macro(miniaudio miniaudio "" lib/cmake/miniaudio)
