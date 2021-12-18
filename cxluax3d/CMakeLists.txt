# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(cxluax3d)

find_package(CXLua)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(IMGui)
find_package(luadbg_static)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Source Files"  FILES ${SOURCES_HEADER_PUB})

add_executable(cxluax3d
  ${PRIVATE_HEADERS}
  src/main.cpp
  ${COMMON_SOURCE_DIR}/script_system.cpp
  ${COMMON_SOURCE_DIR}/file_loading.cpp
  ${COMMON_SOURCE_DIR}/file_system.cpp
  ${COMMON_SOURCE_DIR}/logger.cpp
  ${COMMON_SOURCE_DIR}/imgui/cximgui.cpp
  ${COMMON_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${COMMON_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
  ${COMMON_SOURCE_DIR}/time/timer.cpp
  ${COMMON_SOURCE_DIR}/time/timer_manager.cpp
  src/render/render.cpp
  src/render/shader.cpp
  src/render/camera.cpp
  src/render/texture.cpp

  src/window.cpp
)

if(MSVC)
  target_compile_options(cxluax3d PRIVATE /wd4324 /wd4201 /wd4819  )
else()
  # target_compile_options(SimpleServer PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(cxluax3d
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${COMMON_SOURCE_DIR}
)
target_compile_definitions(cxluax3d 
  PRIVATE 
    -DGLEW_STATIC
    -DIMGUI_IMPL_OPENGL_LOADER_GLEW
    -DWORK_DIR="${ENGINE_DIR}/"
    -DCXLUAX
)

set_target_properties(cxluax3d PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(cxluax3d)
target_link_libraries(cxluax3d
PRIVATE
  glfw
  GLEW
  CXLua::CXLua
  IMGui::IMGui
  luadbg_static
)