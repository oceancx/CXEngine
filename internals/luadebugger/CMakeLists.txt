# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (debugger_all)
include(cmake/cotire.cmake)
include(cmake/HelperMacroFunction.cmake)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()


# list(INSERT CMAKE_PREFIX_PATH 0  ${CMAKE_SOURCE_DIR}/internals)
# list(INSERT CMAKE_PREFIX_PATH 0  ${CMAKE_SOURCE_DIR}/luadbg)

if(NOT TARGET ezio)
    add_subdirectory(internals)
endif()
add_subdirectory(luadbg)
add_subdirectory(vscode)
add_subdirectory(test)