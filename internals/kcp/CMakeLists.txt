cmake_minimum_required(VERSION 3.10)
project(kcp VERSION 1.7 LANGUAGES C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()


add_library(kcp src/ikcp.c)

target_include_directories(kcp
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(kcp kcp "" lib/cmake/kcp) 