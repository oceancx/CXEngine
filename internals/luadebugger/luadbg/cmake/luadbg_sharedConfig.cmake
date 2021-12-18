if(NOT TARGET luadbg_shared)
    find_package(CXLua)
    include("${CMAKE_CURRENT_LIST_DIR}/luadbg_sharedTargets.cmake")
endif()