#ifndef CUBE_SIM_CONFIG_H
#define CUBE_SIM_CONFIG_H

#include "GLFW/glfw3.h"

#define CUBE_LENGTH 3

#define ANIMATION_LENGTH .2f

#define SCR_WIDTH   900
#define SCR_HEIGHT  700

#define MAX_FPS     90

#define MOUSE_SENSITIVITY   .1f

#define ROTATION_KEYS ((unsigned short[]){GLFW_KEY_U,   \
                                          GLFW_KEY_L,   \
                                          GLFW_KEY_F,   \
                                          GLFW_KEY_D,   \
                                          GLFW_KEY_R,   \
                                          GLFW_KEY_B})  \

#define DOUBLE_ROTATION_KEY  GLFW_KEY_SPACE
#define COUNTER_ROTATION_KEY GLFW_KEY_LEFT_CONTROL

#define KEYS_COUNT (sizeof(ROTATION_KEYS) / sizeof(unsigned short))

#define BACKGROUND_COLOR ((vec3){1.f, 1.f, 1.f})

#define UP    ((vec3){1.0f , 1.0f , 0.0f})
#define LEFT  ((vec3){0.0f , 0.0f , 1.0f})
#define FRONT ((vec3){0.73f, 0.0f , 0.0f})
#define DOWN  ((vec3){1.0f , 1.0f , 1.0f})
#define RIGHT ((vec3){0.0f , 0.4f , 0.0f})
#define BACK  ((vec3){1.0f , 0.35f, 0.0f})

#endif // CUBE_SIM_CONFIG_H