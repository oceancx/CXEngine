cmake_minimum_required (VERSION 3.10)
project (CXEngineAll)
include(cmake/HelperMacroFunction.cmake)
include(cmake/cotire.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
list(INSERT CMAKE_PREFIX_PATH 0  ${CMAKE_SOURCE_DIR}/internals)
list(INSERT CMAKE_PREFIX_PATH 0  ${CMAKE_SOURCE_DIR}/internals/luadebugger)
list(INSERT CMAKE_PREFIX_PATH 0  ${CMAKE_SOURCE_DIR}/externals/dist)

add_subdirectory(internals)

set(ENGINE_DIR ${PROJECT_SOURCE_DIR})
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(cxluax)


