# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (asio VERSION 1.18.1)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

add_library(asio INTERFACE)
target_include_directories(asio
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


include(cmake/HelperMacroFunction.cmake)
export_library_macro(asio asio "" lib/cmake/asio)
