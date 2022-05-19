cmake_minimum_required ( VERSION 3.0...3.20 FATAL_ERROR )

project ( CubeSim VERSION 1.0 LANGUAGES C )


set ( EXECUTABLE_NAME cubesim_web )
set ( CMAKE_EXECUTABLE_SUFFIX .js )

# set source and header files
set ( SRC src/main.c src/cubesim_web.c )
set ( HEADERS src/cubesim.h src/cube.h )


# add the executable
add_executable ( ${EXECUTABLE_NAME} ${HEADERS} ${SRC} )
# specify output directory
set_target_properties( ${EXECUTABLE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
    LINK_FLAGS "-s USE_GLFW=3 -s MAX_WEBGL_VERSION=2" )


# cglm options
option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/cglm)
target_link_libraries ( ${EXECUTABLE_NAME} cglm )

include_directories ( ${CMAKE_SOURCE_DIR}/deps/cglm/include )