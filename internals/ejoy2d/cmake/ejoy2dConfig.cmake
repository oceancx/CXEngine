if(NOT TARGET ejoy2d)
    find_package(Lua CONFIG)
    find_package(GLEW CONFIG)
    include("${CMAKE_CURRENT_LIST_DIR}/ejoy2dTargets.cmake")
endif()