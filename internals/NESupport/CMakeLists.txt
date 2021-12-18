cmake_minimum_required(VERSION 3.10)
project(NESupport VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

add_library(nesupport src/NESupport.cpp)

target_include_directories(nesupport
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(nesupport PUBLIC cxx_std_11)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(nesupport NESupport NESupport:: lib/cmake/nesupport)
