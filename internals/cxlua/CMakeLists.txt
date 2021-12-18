cmake_minimum_required(VERSION 3.10)
project(CXLua VERSION 1.0.0 LANGUAGES C CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(LuaCJson)
find_package(EZIO)
find_package(NESupport)
find_package(Nlohmann)
find_package(glm)
find_package(asio)
find_package(kcp)


file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files\\include"  FILES ${SOURCES_HEADER_PUB})

add_library(cxlua
    ${SOURCES_HEADER_PUB}
    ${SOURCES_HEADER_RPI}
    src/lua_net.cpp
    src/net_thread_queue.cpp
    src/cxlua.cpp
    src/utils.cpp
    src/cxmath.cpp
    src/cxrandom.cpp
    src/tsv.cpp
    src/xyq_support.cpp
)

if(MSVC)
  target_compile_options(cxlua PRIVATE /W4 /WX /wd4324 /wd4201 /wd4819)
else()
  target_compile_options(cxlua PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_compile_definitions(cxlua PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(cxlua
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(cxlua PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(cxlua)
target_compile_features(cxlua PUBLIC cxx_std_11)

target_link_libraries(cxlua 
  PUBLIC 
    EZIO::EZIO 
    LuaCJson::LuaCJson
    Nlohmann 
    NESupport::NESupport 
    glm
    asio
    kcp
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(cxlua CXLua CXLua:: lib/cmake/cxlua)
