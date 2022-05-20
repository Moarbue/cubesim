cmake_minimum_required ( VERSION 3.0...3.20 FATAL_ERROR )

project ( CubeSim VERSION 1.0 LANGUAGES C )


set ( EXECUTABLE_NAME cubesim_desktop )
if ( WIN32 )
        set ( CMAKE_EXECUTABLE_SUFFIX .exe )
endif ()

# set source and header files
set ( SRC src/main.c src/cube.c src/shader.c src/cubesim_desktop.c )
set ( HEADERS include/cubesim.h include/cube.h include/shader.h )


# add the executable
add_executable ( ${EXECUTABLE_NAME} ${HEADERS} ${SRC} )
# specify output directory
set_target_properties( ${EXECUTABLE_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin" )


# find OpenGL libraries
find_package ( OpenGL REQUIRED )

# cglm options
option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)

# GLFW options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_VULKAN_STATIC "Assume the Vulkan loader is linked with the application" OFF)


# Add subdirectories of dependencies
add_subdirectory ( deps/cglm        EXCLUDE_FROM_ALL )
add_subdirectory ( deps/glad/cmake  EXCLUDE_FROM_ALL )
add_subdirectory ( deps/glfw        EXCLUDE_FROM_ALL )

# add glad library
glad_add_library ( glad STATIC LOCATION ${CMAKE_BINARY_DIR}/deps/glad LANGUAGE "c" API "gl:core=4.0" )

# Link needed libraries
target_link_libraries ( ${EXECUTABLE_NAME} PRIVATE ${OPENGL_LIBRARIES} )
target_link_libraries ( ${EXECUTABLE_NAME} PRIVATE cglm_headers )
target_link_libraries ( ${EXECUTABLE_NAME} PRIVATE glad )
target_link_libraries ( ${EXECUTABLE_NAME} PRIVATE glfw )

include_directories ( ${CMAKE_SOURCE_DIR}/deps/cglm/include )
include_directories ( ${CMAKE_SOURCE_DIR}/deps/glfw/include )
include_directories ( ${CMAKE_BINARY_DIR}/deps/glad/include )

# copy shaders to bin directory
add_custom_command( TARGET ${EXECUTABLE_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/shaders/ ${CMAKE_SOURCE_DIR}/bin/shaders )