#include <stdio.h>
#include "../include/cubesim.h"

mat4 mproj, mview;
renderdata_t *renderdata;

void render() {
    double start_time = glfwGetTime();

    glfwPollEvents();
    processInput(renderdata);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(renderdata->shader);
    glm_perspective(glm_rad(renderdata->cam.radius), (float)SCR_WIDTH/(float)SCR_HEIGHT, .1f, 1000.f, mproj);
    glUniformMatrix4fv(glGetUniformLocation(renderdata->shader, "projection"), 1, GL_FALSE, &mproj[0][0]);

    camera_get_view_matrix(&renderdata->cam, mview);
    glUniformMatrix4fv(glGetUniformLocation(renderdata->shader, "view"), 1, GL_FALSE, &mview[0][0]);

    cube_draw(renderdata->cube);

    glFlush();

    double end_time = glfwGetTime();
    long remainder = (long)((FRAME_TIME - (end_time - start_time) * 1000));
}

int main() {
    renderdata = (renderdata_t *)malloc(sizeof(renderdata_t));
    int res = setUpWindow(&renderdata->window, SCR_WIDTH, SCR_HEIGHT, "CubeSim");

    if (res == 0) {
        printf("Successfully created context window.\n");
    } else if (res == -1) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW.\nTerminating...\n");
        return 1;
    } else if (res == -2) {
        fprintf(stderr, "ERROR: Failed to initialize Window.\nTerminating...\n");
        glfwTerminate();
        return 1;
    }

    res = shader_new(&renderdata->shader, VERTEX_PATH, FRAGMENT_PATH);
    if (res == 0) {
        printf("Initialized Shaders.\n");
        fflush(stdout);
    } else if (res == -1) {
        fprintf(stderr, "ERROR: Failed to open files. Specified paths:\n\t%s\n\t%s\nTerminating...\n", VERTEX_PATH, FRAGMENT_PATH);
        return 1;
    } else if (res == -2) {
        fprintf(stderr, "ERROR: Failed to read files. Specified paths:\n\t%s\n\t%s\nTerminating...\n", VERTEX_PATH, FRAGMENT_PATH);
        return 1;
    }

    renderdata->cube = cube_new((vec3){0.0f, 0.f, 0.f}, renderdata->shader);
    renderdata->cam = camera_new((vec3){0.f, 1.f, 3.f}, (vec3){0.f, 0.f, 0.f}, Y_AXIS, 0.f, 0.f, 30.f);
    
    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1.0f);
    glEnable(GL_DEPTH_TEST);

    mainloop(&render, renderdata);

    printf("Exited main loop successfully.\n");

    glfwDestroyWindow(renderdata->window);
    cube_delete(renderdata->cube);
    free(renderdata);
    return 0;
}