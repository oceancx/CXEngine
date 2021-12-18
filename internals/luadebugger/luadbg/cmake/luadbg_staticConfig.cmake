if(NOT TARGET luadbg_static)
    find_package(CXLua)
    include("${CMAKE_CURRENT_LIST_DIR}/luadbg_staticTargets.cmake")
endif()