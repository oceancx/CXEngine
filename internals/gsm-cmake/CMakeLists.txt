cmake_minimum_required (VERSION 3.10)
project(gsm VERSION 1.0 LANGUAGES C CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()


file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files\\include"  FILES ${SOURCES_HEADER_PUB})

set(GSM_SOURCES
src/add.c
src/code.c
src/debug.c
src/decode.c
src/long_term.c
src/lpc.c
src/preprocess.c
src/rpe.c
src/gsm_destroy.c
src/gsm_decode.c
src/gsm_encode.c
src/gsm_explode.c
src/gsm_implode.c
src/gsm_create.c
src/gsm_print.c
src/gsm_option.c
src/short_term.c
src/table.c
)
add_library(gsm
    ${SOURCES_HEADER_PUB}
    ${SOURCES_HEADER_RPI}
   ${GSM_SOURCES}
)
target_compile_definitions(gsm PUBLIC -DWAV49)
target_include_directories(gsm
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


include(cmake/HelperMacroFunction.cmake)
export_library_macro(gsm gsm "" lib/cmake/gsm)
