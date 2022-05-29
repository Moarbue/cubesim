#include "../include/cubesim.h"
#include <emscripten/emscripten.h>

// get the next move from the website
EM_JS(int, getMove, (int *_move), {
    let tmp = getMoveJS();
    setValue(_move, tmp[0], "i32"); 
    return tmp[1];
});

// get the fov angle from the website
EM_JS(int, getFov, (float *_fov), {
    let tmp = getFovJS();
    setValue(_fov, tmp[0], "float");
    return tmp[1];
});

// get the cursor position from the website
EM_JS(int, getCursorPos, (int *_pos), {
    let tmp = getCursorPosJS();
    setValue(_pos,   tmp[0], "i32"); 
    setValue(_pos+4, tmp[1], "i32"); 
    return tmp[2];
});

// get the current algorithm from the website
EM_JS(char *, getAlgorithm, (int *_flag), {
    let tmp = getAlgorithmJS();
    let len = (Module.lengthBytesUTF8(tmp[0]) + 1);   
    const str = Module._malloc(len);
    Module.stringToUTF8(tmp[0], str, len);

    setValue(_flag, tmp[1], "i32");

    return str;
})

// Get the current faces state from the website
EM_JS(char *, getFaces, (int *_flag), {
    let tmp = getFacesJS();
    let len = (Module.lengthBytesUTF8(tmp[0]) + 1);   
    const str = Module._malloc(len);
    Module.stringToUTF8(tmp[0], str, len);

    setValue(_flag, tmp[1], "i32");

    return str;
})

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    glm_perspective_resize((float)width/(float)height, mproj);
}

int setUpWindow(GLFWwindow **window, const unsigned int width, const unsigned int height, const char *title)
{
    if (glfwInit() == GLFW_FALSE) return -1; // GLFW-Initialization failed

    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // OpenGL profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // Specify ES API
    glfwWindowHint(GLFW_OPENGL_API, GLFW_OPENGL_ES_API);

    // Disable fixed fps
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    // Create window
    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (*window == NULL) return -2; // Window creation failed

    // Make window context current
    glfwMakeContextCurrent(*window);

    return 0;
}

void mainloop(renderfunc render, renderdata_t *_renderdata)
{
    emscripten_set_main_loop_arg((em_arg_callback_func) render, _renderdata, MAX_FPS, 1);
}

void processInput(renderdata_t *_renderdata)
{
    // Variable that shows if data changed since last check
    int flag = 0;

    // get the next move
    int move = 0;
    flag = getMove(&move);

    if (flag) {
        cube_execute_move(renderdata->cube, move);
    }


    // get the fov angle (zoom)
    float fov = 0;
    flag = getFov(&fov);

    if (flag) {
        camera_set_angles(&_renderdata->cam, _renderdata->cam.yaw, _renderdata->cam.pitch, fov);
    }


    // get the cursor position (rotation of whole cube)
    int cursorPos[2] = { 0 };
    flag = getCursorPos(cursorPos);

    if (flag) {
        camera_set_angles(&_renderdata->cam, cursorPos[1] * MOUSE_SENSITIVITY, cursorPos[0] * MOUSE_SENSITIVITY, _renderdata->cam.radius);
    }


    // get the next executed algorithm
    char *algorithm = getAlgorithm(&flag);

    if (flag && algorithm != NULL) {
        cube_execute_algorithm(renderdata->cube, algorithm);
    }
    free(algorithm);


    // get the state of the cube on the face level
    char *faces = getFaces(&flag);

    if (flag && faces != NULL) {
        renderdata->cube = cube_from_face_cube((vec3){0.f, 0.f, 0.f}, faces, renderdata->shader);
    }
    free(faces);
}