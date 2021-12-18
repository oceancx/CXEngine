if(NOT TARGET EZIO::EZIO)
    find_package(KBase)
    include("${CMAKE_CURRENT_LIST_DIR}/EZIOTargets.cmake")
endif()