# CMake entry point
cmake_minimum_required (VERSION 3.1)
project (nlohmann VERSION 1.0.0)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

add_library(nlohmann INTERFACE)
target_include_directories(nlohmann
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


include(cmake/HelperMacroFunction.cmake)
export_library_macro(nlohmann Nlohmann "" lib/cmake/nlohmann)
