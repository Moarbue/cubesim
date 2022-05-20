#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/cglm.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef struct camera_t {
    vec3 eye;
    vec3 target;
    vec3 up;

    float pitch;
    float yaw;
    float radius;

} camera_t;

/// @brief calculates the position vector of a given camera
/// @param cam the camera
static inline void camera_calculate_vectors(camera_t *cam) {
    
    vec3 cam_dir;
    glm_vec3_sub(cam->target, cam->eye, cam_dir);
    glm_normalize(cam_dir);

    vec3 cam_right;
    glm_vec3_cross(cam->up, cam_dir, cam_right);
    glm_normalize(cam_right);

    float cos_angle = glm_vec3_dot(cam_dir, cam->up);

    if (cos_angle * sin(glm_rad(cam->pitch)) > .99f) cam->pitch = 0.f;

    glm_vec3_sub(cam->eye, cam->target, cam->eye);
    glm_vec3_rotate(cam->eye, glm_rad(cam->yaw), cam->up);

    glm_vec3_sub(cam->eye, cam->target, cam->eye);
    glm_vec3_rotate(cam->eye, glm_rad(cam->pitch), cam_right);
}

/// @brief Create a new camera with the specified values.
/// @param eye the position of the camera, default 1,1,1
/// @param target the target the camera is looking at, default 0,0,0
/// @param up the up vector in the world, default 0,1,0
/// @param pitch the pitch-angle of the camera
/// @param yaw the yaw angle of the camera
/// @param radius the radius of the camera where the center point is the target
static inline camera_t camera_new(vec3 eye, vec3 target, vec3 up, float pitch, float yaw, float radius) {
    camera_t cam;

    if (eye == NULL)    glm_vec3_copy(GLM_VEC3_ONE, cam.eye);
    else                glm_vec3_copy(eye, cam.eye);

    if (target == NULL) glm_vec3_copy(GLM_VEC3_ZERO, cam.target);
    else                glm_vec3_copy(target, cam.target);

    if (up == NULL)     glm_vec3_copy(GLM_YUP, cam.up);
    else                glm_vec3_copy(up, cam.up);
    
    
    cam.pitch  = pitch;
    cam.yaw    = yaw;
    cam.radius = radius;

    camera_calculate_vectors(&cam);

    return cam;
}

/// @brief set the pitch and yaw angles and the radius of a camera
/// @param pitch the pitch-angle of the camera
/// @param yaw the yaw angle of the camera
/// @param radius the radius of the camera where the center point is the target
static inline void camera_set_angles(camera_t *cam, float pitch, float yaw, float radius) {
    cam->pitch  = pitch;
    cam->yaw    = yaw;
    cam->radius = radius;

    camera_calculate_vectors(cam);
}

/// @brief calculates the view matrix
/// @param cam the camera to get the view matrix from
/// @param mview the resulting view matrix
static inline void camera_get_view_matrix(camera_t *cam, mat4 mview) {
    glm_lookat(cam->eye, cam->target, cam->up, mview);
}

#endif // CAMERA_H