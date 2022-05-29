#ifndef CUBE_H
#define CUBE_H

#define GLFW_INCLUDE_NONE
#include "../config.h"
#include "cglm/cglm.h"

// Predefines indices of a cubie
#define INDICES ((unsigned short[]){ 0,  1,  3,      1,  2,  3,    \
                                     4,  5,  7,      5,  6,  7,    \
                                     4,  0,  7,      0,  3,  7,    \
                                     1,  5,  2,      5,  6,  2,    \
                                     4,  5,  0,      5,  1,  0,    \
                                     3,  2,  7,      2,  6,  3,    \
                                                                   \
                                     8,  9, 11,      9, 10, 11,    \
                                    12, 13, 15,     13, 14, 15,    \
                                    16, 17, 19,     17, 18, 19 })  \

// rotation axises
#define X_AXIS ((vec3){1.f, 0.f, 0.f})
#define Y_AXIS ((vec3){0.f, 1.f, 0.f})
#define Z_AXIS ((vec3){0.f, 0.f, 1.f})

// Colors, face colors can be modified in ../config.h
#define BLACK ((vec3){0.0f , 0.0f , 0.0f})

#define COLORS ((float*[]){UP, LEFT, FRONT, DOWN, RIGHT, BACK})

// side length of a cubie
#define CUBIE_SIZE (.3f)
// sticker size of a cubie
#define STICKER_SIZE (CUBIE_SIZE * .95f)
// sticker offset from cubie corner to sticker corner
#define STICKER_OFFSET ((CUBIE_SIZE - STICKER_SIZE))
// offset from cubie to sticker
#define OFFSET 0.0001f

// *** Cube Length is defined in ../config.h
// count of all cubies (e.g 3x3x3 = 27 cubies)
#define CUBIE_COUNT (CUBE_LENGTH * CUBE_LENGTH * CUBE_LENGTH)
// count of cubies per face (e.g 3x3 = 9 cubies)
#define CUBIES_PER_FACE (CUBE_LENGTH * CUBE_LENGTH)

// relative coordinates of a sticker
// UP, LEFT, FRONT
#define COLORED_FACE_COORDS ((float*[]){                                                                \
    (vec3){STICKER_OFFSET, +OFFSET, -(STICKER_SIZE)}, (vec3){STICKER_SIZE, +OFFSET, -(STICKER_SIZE)},   \
    (vec3){STICKER_SIZE, +OFFSET, -STICKER_OFFSET},   (vec3){STICKER_OFFSET, +OFFSET, -STICKER_OFFSET}, \
                                                                                                        \
    (vec3){-OFFSET, -STICKER_OFFSET, -(STICKER_SIZE)},(vec3){-OFFSET, -STICKER_OFFSET, -STICKER_OFFSET},\
    (vec3){-OFFSET, -STICKER_SIZE, -STICKER_OFFSET},  (vec3){-OFFSET, -STICKER_SIZE, -(STICKER_SIZE)},  \
                                                                                                        \
    (vec3){STICKER_OFFSET, -STICKER_OFFSET, +OFFSET}, (vec3){STICKER_SIZE, -STICKER_OFFSET, +OFFSET},   \
    (vec3){STICKER_SIZE, -STICKER_SIZE, +OFFSET},     (vec3){STICKER_OFFSET, -STICKER_SIZE, +OFFSET}})  \

// *** macro functions to check if a cubie belongs to a certain face
#define CUBE_IS_UP(_index)      (((_index) % CUBIES_PER_FACE) < CUBE_LENGTH)
#define CUBE_IS_LEFT(_index)    (((_index) % CUBE_LENGTH) == 0)
#define CUBE_IS_FRONT(_index)   (((_index) / CUBIES_PER_FACE) == 0)
#define CUBE_IS_DOWN(_index)    (((_index) % CUBIES_PER_FACE) >= ((CUBE_LENGTH - 1) * CUBE_LENGTH))
#define CUBE_IS_RIGHT(_index)   (((_index) % CUBE_LENGTH) == (CUBE_LENGTH - 1))
#define CUBE_IS_BACK(_index)    (((_index) / CUBIES_PER_FACE) == (CUBE_LENGTH - 1))

// rotation angle per step
#define ROTATION_FACTOR (90.f/(ANIMATION_LENGTH * 10.f))
#define STEPS_PER_QUARTER (90.f/ROTATION_FACTOR)
#define STEPS_PER_HALF (180.f/ROTATION_FACTOR)
// how often the animation is updated
#define ANIMATION_INTERVALL (ANIMATION_LENGTH / MAX_FPS) // 60 FPS

// Enum representing all the cube faces
typedef enum faces_t {
    U,L,F,D,R,B
} faces_t;

// Enum representing all the 18 moves
typedef enum moves_t {
    Ux1,Lx1,Fx1,Dx1,Rx1,Bx1,
    Ux2,Lx2,Fx2,Dx2,Rx2,Bx2,
    Ux3,Lx3,Fx3,Dx3,Rx3,Bx3
} moves_t;

// vertex = position + color
struct vertex_t;

// cubie cube struct
struct cubie_t;

// cube struct
struct cube_t;


// ** cubie functions

/// @brief Generates the vertices of a cubie with the corresponding colored faces
/// @param c the cubie for which the vertices are generated
/// @param pos array of faces_t specifies the position of the colored faces U = up, etc.
void cubie_calcverts(struct cubie_t *c, faces_t *pos);

/// @brief Generates a cubie given the position, and the colored faces. Vertices are calculated automatically
/// @param pos the position of the cubie in the world
/// @param colored_faces array of colored faces. 
/// order of faces is {U, L, F, D, R, B}, for example {U, D, R} is ok but {F, D, U} is wrong
/// @param colface_cnt count of colored faces
/// @return the generated cubie
struct cubie_t *cubie_new(vec3 pos, faces_t *colored_faces, unsigned short colface_cnt);

/// @brief Draws a cubie to the screen, you need to first enable the shader program like glUseProgram(<shader>)
/// @param c the cubie that is drawn
/// @param mmodel the model matrix for the cubie
/// @param modelPos the uniform location of the model matrix in the shader
void cubie_draw(struct cubie_t c, mat4 mmodel, unsigned int modelPos);

/// @brief Frees the memory used by a cubie
/// @param c the cubie to delete
void cubie_delete(struct cubie_t *c);


// ** cube functions

/// @brief Generates a cube with variable side length (Specify in config.h)
/// @param origin the origin of the cube
/// @param shader the shader used to render the vertices, needed to get location of model matrix
/// @return the generated cube
struct cube_t *cube_new(vec3 origin, unsigned int shader);

/// @brief executes a move on a cube
/// @param c the cube to execute the move on
/// @param move on of the 18 possible moves (Ux1, Ux2, Ux3, Lx1,...)
void cube_execute_move(struct cube_t *c, moves_t move);

/// @brief Draws a cube in it's correct state to the screen
/// @param c the cube to draw
void cube_draw(struct cube_t *c);

/// @brief Generates a cube from a string. Note: You need to first initialize the cube with cube_new() function.
/// Format of string: UUU...RRR...FFF..DDD...LLL...BBB... where the first U is the color of the top left face on the up-side.
/// example string: "DUUBULDBFRBFRRULLLBRDFFFBLURDBFDFDRFRULBLUFDURRBLBDUDL"
/// @param c the cube to generate
/// @param face_cube the string to generate from, see example string
struct cube_t *cube_from_face_cube(vec3 origin, const char *face_cube, unsigned int shader);

/// @brief Adds a algorithm to execute to a cube
/// @param c the cube the algorithm is executed on
/// @param algorithm the algorithm to execute, e.g UL'D2 (extra characters are not removed)
void cube_execute_algorithm(struct cube_t *c, char *algorithm);

/// @brief Frees the memory used by a cube
/// @param c the cube to delete
void cube_delete(struct cube_t *c);

#endif