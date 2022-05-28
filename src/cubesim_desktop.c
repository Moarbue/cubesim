#include "../include/cubesim.h"

#ifdef _WIN32
#   include <windows.h>
#else
#   include <unistd.h>
#endif

double old_x = 0.0, old_y = 0.0;

// the function that is called when a mouse button is pressed.
void cursorPosCallback(GLFWwindow *window, double x, double y)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera_set_angles(&renderdata->cam, (y - old_y) * MOUSE_SENSITIVITY, (old_x - x) * MOUSE_SENSITIVITY, renderdata->cam.radius);
    }

    old_x = x;
    old_y = y;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    glm_perspective_resize((float)width/(float)height, mproj);
}

double last_yoffset = 0.0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{   
    float fov = renderdata->cam.radius;
    if (fov >= 1.0f && fov <= 90.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 90.0f)
        fov = 90.0f;

    camera_set_angles(&renderdata->cam, renderdata->cam.pitch, renderdata->cam.yaw, fov);
}


int setUpWindow(GLFWwindow **window, const unsigned int width, const unsigned int height, const char *title)
{
    if (glfwInit() == GLFW_FALSE) return -1; // GLFW-Initialization failed

    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // OpenGL profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    // Disable fixed fps
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    // Create window
    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (*window == NULL) return -2; // Window creation failed

    // Make window context current
    glfwMakeContextCurrent(*window);
    // Set function that is called when window is resized
    glfwSetFramebufferSizeCallback(*window, frameBufferSizeCallback);
    // Set function that is called when a mouse button is pressed
    glfwSetCursorPosCallback(*window, cursorPosCallback);
    // Set function that is called when the mouse wheel is scrolled
    glfwSetScrollCallback(*window, scroll_callback);

    return 0;
}

int setUpOpenGL()
{
    if (!gladLoadGL(glfwGetProcAddress)) return -1;
    return 0;
}

void mainloop(renderfunc render, renderdata_t *_renderdata)
{
    while (!glfwWindowShouldClose(_renderdata->window))
    {
        render(_renderdata);
    }
}

// Check if any of the Rotation keys where pressed and execute a move if so
void checkRotationKeys(renderdata_t *_renderdata, unsigned int moveType) {
    if ( _renderdata->lastKey[0] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[0]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Ux1 + moveType * 6);

    if ( _renderdata->lastKey[1] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[1]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Lx1 + moveType * 6);

    if ( _renderdata->lastKey[2] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[2]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Fx1 + moveType * 6);

    if ( _renderdata->lastKey[3] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[3]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Dx1 + moveType * 6);

    if ( _renderdata->lastKey[4] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[4]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Rx1 + moveType * 6);

    if ( _renderdata->lastKey[5] == GLFW_RELEASE && glfwGetKey(_renderdata->window, ROTATION_KEYS[5]) == GLFW_PRESS )
        cube_execute_move(_renderdata->cube, Bx1 + moveType * 6);
}

void processInput(renderdata_t *_renderdata)
{
    if ( glfwGetKey(_renderdata->window, GLFW_KEY_ESCAPE) == GLFW_PRESS )
        glfwSetWindowShouldClose(_renderdata->window, GLFW_TRUE);

    if (glfwGetKey(_renderdata->window, DOUBLE_ROTATION_KEY) == GLFW_PRESS)
        checkRotationKeys(_renderdata, 1);
    else if (glfwGetKey(_renderdata->window, COUNTER_ROTATION_KEY) == GLFW_PRESS)
        checkRotationKeys(_renderdata, 2);
    else
        checkRotationKeys(_renderdata, 0);


    for (unsigned short i = 0; i < KEYS_COUNT; i++) {
        _renderdata->lastKey[i] = glfwGetKey(_renderdata->window, ROTATION_KEYS[i]);
    }
}


void sleep_millis(unsigned long millis) {
    #ifdef _WIN32
        Sleep(millis);
    #else
        usleep(millis * 1000);
    #endif
}