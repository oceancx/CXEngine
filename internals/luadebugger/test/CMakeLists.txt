cmake_minimum_required (VERSION 3.10)
project (luadbgtest VERSION 1.0.0)

find_package(luadbg_static)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" "include/*.inl" "include/*.lua")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_executable(luadbgtest
  ${SOURCES_HEADER_RPI}
  src/main.cpp
)

target_include_directories(luadbgtest
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(luadbgtest 
  luadbg_static
)
#target_compile_definitions(luadbgtest PUBLIC -LUADBG_DLL)
set_target_properties(luadbgtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})

# add_custom_command(TARGET luadbgtest
#           POST_BUILD
#           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>/luadbgtest.exe ${CMAKE_CURRENT_LIST_DIR}
# )