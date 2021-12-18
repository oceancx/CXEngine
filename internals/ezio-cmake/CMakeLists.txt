cmake_minimum_required(VERSION 3.5)
project(EZIO VERSION 1.0.0 LANGUAGES CXX)

#########################################################################
# configure_file(${CMAKE_SOURCE_DIR}/cmake/KBase-CMakeLists.txt.in ${CMAKE_BINARY_DIR}/kbase-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/kbase-download )
# if(result)
#   message(FATAL_ERROR "CMake step for kbase-download failed: ${result}")
# endif()

# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/kbase-download )
# if(result)
#   message(FATAL_ERROR "Build step for kbase-download failed: ${result}")
# endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

find_package(KBase)  # PATHS "../kbase-cmake/dist"

add_library(ezio  
    src/ezio/acceptor.cpp
    src/ezio/buffer.cpp
    src/ezio/connector_base.cpp
    src/ezio/event_loop.cpp
    src/ezio/event_pump.cpp
    src/ezio/io_service_context.cpp
    src/ezio/notifier.cpp
    src/ezio/socket_address.cpp
    src/ezio/socket_utils.cpp
    src/ezio/tcp_server.cpp
    src/ezio/tcp_client.cpp
    src/ezio/tcp_connection.cpp
    src/ezio/timer.cpp
    src/ezio/timer_queue.cpp
    src/ezio/this_thread.cpp
    src/ezio/thread.cpp
    src/ezio/worker_pool.cpp 
)

if(WIN32)
    target_sources(ezio PRIVATE    
        src/ezio/acceptor_win.cpp
        src/ezio/connector_win.cpp
        src/ezio/event_pump_impl_win.cpp
        src/ezio/notifier_win.cpp
        src/ezio/socket_utils_win.cpp 
        src/ezio/tcp_connection_win.cpp
        src/ezio/winsock_context.cpp
    )
elseif(APPLE)
    target_sources(ezio PRIVATE    
        src/ezio/acceptor_apple.cpp
        src/ezio/buffer_posix.cpp
        src/ezio/connector_posix.cpp
        src/ezio/event_pump_impl_apple.cpp
        src/ezio/ignore_sigpipe.cpp
        src/ezio/notifier_posix.cpp
        src/ezio/socket_utils_posix.cpp 
        src/ezio/tcp_connection_posix.cpp
    )
elseif(UNIX)
    target_sources(ezio PRIVATE    
        src/ezio/acceptor_posix.cpp
        src/ezio/buffer_posix.cpp
        src/ezio/connector_posix.cpp
        src/ezio/event_pump_impl_posix.cpp
        src/ezio/ignore_sigpipe.cpp
        src/ezio/notifier_posix.cpp
        src/ezio/socket_utils_posix.cpp 
        src/ezio/tcp_connection_posix.cpp
    )
endif()

target_include_directories(ezio
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(ezio PUBLIC cxx_std_14)
target_link_libraries(ezio PUBLIC KBase::KBase)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(ezio EZIO EZIO:: lib/cmake/ezio)
