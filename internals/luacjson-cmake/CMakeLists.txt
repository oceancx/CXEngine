cmake_minimum_required(VERSION 3.10)
project(lua-cjson VERSION 1.0.3 LANGUAGES C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(Lua CONFIG)

add_library(cjson src/lua_cjson.c src/strbuf.c)
target_compile_definitions(cjson PUBLIC -DVERSION="${PROJECT_VERSION}")

target_link_libraries(cjson PUBLIC Lua::Lua)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(cjson LuaCJson LuaCJson:: lib/cmake/luacjson)
