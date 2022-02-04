# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(cxluax)

find_package(CXLua)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(IMGui)


file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp"   "src/*.cc")
file(GLOB_RECURSE  SOURCES_HEADER_PRI "src/*.h"  "src/*.hpp" "src/*.inl")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_HEADER_PRI})

add_executable(cxluax
  ${SOURCES_HEADER_PRI}
  src/main.cpp
  src/coordinate.cpp
  ${COMMON_SOURCE_DIR}/script_system.cpp
  ${COMMON_SOURCE_DIR}/file_loading.cpp
  ${COMMON_SOURCE_DIR}/file_system.cpp
  ${COMMON_SOURCE_DIR}/logger.cpp
  ${COMMON_SOURCE_DIR}/imgui/cximgui.cpp
  ${COMMON_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${COMMON_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
  ${COMMON_SOURCE_DIR}/time/timer.cpp
  ${COMMON_SOURCE_DIR}/time/timer_manager.cpp
  src/window.cpp
  src/net.cpp
)

if(MSVC)
  target_compile_options(cxluax PRIVATE /wd4324 /wd4201 /wd4819  )
else()
  # target_compile_options(SimpleServer PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(cxluax
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${COMMON_SOURCE_DIR}
)
target_compile_definitions(cxluax 
  PRIVATE 
    -DLUADBG_DLL
    -DGLEW_STATIC
    -DIMGUI_IMPL_OPENGL_LOADER_GLEW
    -DWORK_DIR="${ENGINE_DIR}/"
    -DCXLUAX
    -DWIN32_LEAN_AND_MEAN
)

set_target_properties(cxluax PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(cxluax)
target_link_libraries(cxluax
PRIVATE
  glfw
  GLEW
  CXLua::CXLua
  IMGui::IMGui
)