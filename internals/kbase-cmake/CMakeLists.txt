cmake_minimum_required(VERSION 3.5)
project(KBase VERSION 1.0.0 LANGUAGES CXX)
include(cmake/cotire.cmake)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

add_library(kbase  
    src/kbase/at_exit_manager.cpp
    src/kbase/base64.cpp
    src/kbase/command_line.cpp
    src/kbase/chrono_util.cpp
    src/kbase/error_exception_util.cpp
    src/kbase/file_iterator.cpp
    src/kbase/file_util.cpp
    src/kbase/guid.cpp
    src/kbase/logging.cpp
    src/kbase/md5.cpp
    src/kbase/os_info.cpp
    src/kbase/path.cpp
    src/kbase/path_service.cpp
    src/kbase/pickle.cpp
    src/kbase/string_encoding_conversions.cpp
    src/kbase/string_format.cpp
    src/kbase/string_util.cpp
)

if(WIN32)
    target_sources(kbase PRIVATE    
        src/kbase/base_path_provider_win.cpp
        src/kbase/debugger_win.cpp
        src/kbase/environment_win.cpp
        src/kbase/file_iterator_win.cpp
        src/kbase/file_util_win.cpp
        src/kbase/os_info_win.cpp
        src/kbase/stack_walker_win.cpp
    )
elseif(UNIX)
   target_sources(kbase PRIVATE
        src/kbase/base_path_provider_posix.cpp 
        src/kbase/debugger_posix.cpp 
        src/kbase/environment_posix.cpp 
        src/kbase/file_iterator_posix.cpp 
        src/kbase/file_util_posix.cpp 
        src/kbase/os_info_posix.cpp 
        src/kbase/stack_walker_posix.cpp
    )
elseif(APPLE)
    target_sources(kbase PRIVATE
        src/kbase/base_path_provider_posix.cpp 
        src/kbase/debugger_posix.cpp 
        src/kbase/environment_posix.cpp 
        src/kbase/file_iterator_posix.cpp 
        src/kbase/file_util_posix.cpp 
        src/kbase/os_info_posix.cpp 
        src/kbase/stack_walker_posix.cpp
    )
endif()

target_include_directories(kbase
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    target_compile_definitions(kbase 
        PUBLIC
            -DNOMINMAX 
            -DUNICODE 
            -DWIN32_LEAN_AND_MEAN
    )
elseif(APPLE)
    target_compile_definitions(kbase 
        PUBLIC
            -D_POSIX_C_SOURCE=200809L
        )
endif()

target_compile_features(kbase PUBLIC cxx_std_14)
set_target_properties(kbase PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(kbase)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(kbase KBase KBase:: lib/cmake/kbase)
