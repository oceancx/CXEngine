if(NOT TARGET LuaCJson::LuaCJson)
    find_package(Lua CONFIG)    
    include("${CMAKE_CURRENT_LIST_DIR}/LuaCJsonTargets.cmake")
endif()
