cmake_minimum_required (VERSION 3.10)
project (luadbg VERSION 1.0.0 LANGUAGES C CXX)

find_package(CXLua)


file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" "include/*.inl" "include/*.lua")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_library(luadbg_static
  ${SOURCES_HEADER_PUB}
  ${SOURCES_HEADER_PRI}
  src/luadbg.cpp
)
target_compile_definitions(luadbg_static 
  PRIVATE 
    -D_LUADBG_BUILD_DLL
    -DVERSION="${PROJECT_VERSION}"
    -DLUA_COMPAT_5_2 
    -DLUA_COMPAT_5_1
)


target_include_directories(luadbg_static
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(luadbg_static
  PUBLIC
    CXLua::CXLua
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(luadbg_static luadbg_static "" lib/cmake/luadbg)

set_target_properties(luadbg_static PROPERTIES LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(luadbg_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(luadbg_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})


# add_custom_command(TARGET luadbg_static
#           PRE_BUILD
#           COMMAND sh ${CMAKE_CURRENT_LIST_DIR}/generate_inl.sh ${CMAKE_CURRENT_LIST_DIR}
# )

# add_custom_command(TARGET luadbg
#           POST_BUILD
#           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>/luadbg.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
#           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>/luadbg.lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
# )

add_library(luadbg_shared SHARED
  ${SOURCES_HEADER_PUB}
  ${SOURCES_HEADER_PRI}
  src/luadbg.cpp
)

target_compile_definitions(luadbg_shared 
  PRIVATE 
    -D_LUADBG_BUILD_DLL
    -DVERSION="${PROJECT_VERSION}"
    -DLUA_COMPAT_5_2 
    -DLUA_COMPAT_5_1
)

target_include_directories(luadbg_shared
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(luadbg_shared
  PUBLIC
    CXLua::CXLua
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(luadbg_shared luadbg_shared "" lib/cmake/luadbg)

set_target_properties(luadbg_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(luadbg_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(luadbg_shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})


# add_custom_command(TARGET luadbg_shared
#           PRE_BUILD
#           COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/generate_inl.sh ${CMAKE_CURRENT_LIST_DIR}
# )