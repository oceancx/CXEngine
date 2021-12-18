file(GLOB_RECURSE  _GAME_SOURCES_CPP "src/*.cpp"  )
file(GLOB_RECURSE  _GAME_SOURCES_HEADER "src/*.h"  "src/*.hpp" "src/*.inl" )
set(GAME_SOURCES_CPP ${_GAME_SOURCES_CPP} PARENT_SCOPE)
set(GAME_SOURCES_HEADER ${_GAME_SOURCES_HEADER} PARENT_SCOPE)

set(GAME_COMMON_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/cximgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/time/time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/time/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/time/timer_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_loading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/script_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/astar/astar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actor/action.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actor/actor_enum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actor/actor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actor/actor_prop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/actor/actor_manager.cpp
    PARENT_SCOPE
)

set(COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)