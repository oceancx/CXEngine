# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (vsdbgadapter)
set(extentiondir ${CMAKE_CURRENT_SOURCE_DIR}) 

find_package(CXLua)
find_package(luadbg_static)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h"  "src/*.inl")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})

add_executable(vsdbgadapter
  ${SOURCES_HEADER_RPI}
  src/main.cpp
)

target_include_directories(vsdbgadapter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vsdbgadapter
  PUBLIC 
	CXLua::CXLua
	luadbg_static
)

set_target_properties(vsdbgadapter PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${extentiondir})

add_custom_command(TARGET vsdbgadapter
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${extentiondir}/$<CONFIG>/vsdbgadapter.exe ${extentiondir}/
)

# configure_file(main.lua  ${extentiondir}/main.lua COPYONLY)
# configure_file(threads.lua     ${extentiondir}/threads.lua COPYONLY)
# configure_file(package.json     ${extentiondir}/package.json COPYONLY)