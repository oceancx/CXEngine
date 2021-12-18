cmake_minimum_required(VERSION 3.10)
project(IMGui VERSION 1.0.0 LANGUAGES C CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(Lua CONFIG)
add_library(imgui
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    # extra/imgui_impl.cpp
    # extra/wrap_imgui_impl.cpp
)

target_include_directories(imgui
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(imgui Lua::Lua)
include(cmake/HelperMacroFunction.cmake)
export_library_macro(imgui IMGui IMGui:: lib/cmake/imgui)

