#ifndef CUBE_SIM_H
#define CUBE_SIM_H

#define GLFW_INCLUDE_NONE
#include "GLES3/gl3.h"
#include <emscripten/emscripten.h>
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "shader.h"
#include "cube.h"
#include "camera.h"
#include "../config.h"

#define VERTEX_PATH "shaders/default.vert"
#define FRAGMENT_PATH "shaders/default.frag"

#define FRAME_TIME (unsigned long)((1.0/(double)MAX_FPS) * 1000)

extern mat4 mproj, mview;

/// @brief Creates a glfw-window for OpenGL to draw to.
/// @param window the window to create
/// @param width the width of the window
/// @param height the height of the window
/// @param title the title of the window
/// @retval 0 = success
/// @retval -1 = failed to initialize glfw
/// @retval -2 = failed to initialize window. Remember to Terminate GLFW
int setUpWindow(GLFWwindow **window, const unsigned int width, const unsigned int height, const char *title);

/// @brief Loads the OpenGL/OpenGL ES functions
/// @retval 0 = success
/// @retval -1 = failed to initialize OpenGL. Remember to terminate GLFW
int setUpOpenGL();

// the function that is called when the window is resized.
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);

typedef struct renderdata_t {
    GLFWwindow *window;
    unsigned int shader;
    struct cube_t *cube;
    camera_t cam;
} renderdata_t;

extern renderdata_t *renderdata;

typedef void (*renderfunc)(renderdata_t *renderdata);

/// @brief Main loop of the program
/// @param window the context window
/// @param render the render function which is called every frame
void mainloop(renderfunc render, renderdata_t *_renderdata);

// This functions handles every keyboard-input from the user
void processInput(renderdata_t *_renderdata);

#endif // CUBE_SIM_WEB_H