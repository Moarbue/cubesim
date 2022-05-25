#include "../include/cube.h"
#include <string.h>
#include <GLES3/gl3.h>
#include "GLFW/glfw3.h"

typedef struct vertex_t {
    vec3 pos;
    vec3 color;
} vertex_t;

typedef struct cubie_t {
    vec3 pos;
    faces_t *colored_faces;
    unsigned short colface_cnt;
    vertex_t *verts; // 4 corners per face * 6 faces = 24
    unsigned short verts_cnt;
    unsigned short inds_cnt;
    unsigned int VAO;
    mat4 mmodel;
} cubie_t;

typedef struct cube_t {
    vec3 origin;
    cubie_t c[CUBIE_COUNT];
    unsigned int mmodel_loc;
    unsigned short rot_indices[CUBIES_PER_FACE];
    double last_rot;
    unsigned short cur_step;
    float rot_step;
    unsigned short is_rotating;
    vec3 rotAxis;
    moves_t *algorithm;
    int algo_len;
} cube_t;


void cubie_calcverts(cubie_t *c, faces_t *pos) {
    
    c->inds_cnt = (c->colface_cnt * 6 +36);
    // 8 outer vertices + 4 vertices per colored face
    c->verts_cnt = (8 + c->colface_cnt * 4);
    c->verts = (vertex_t *)malloc(sizeof(vertex_t) * c->verts_cnt);
    
    // generate the 8 outer corner vertices
    for (int i = 0; i < 8; i++) {
        glm_vec3_copy(BLACK, c->verts[i].color);
        glm_vec3_copy(c->pos, c->verts[i].pos);

        if (i % 4 == 1 || i % 4 == 2)
            c->verts[i].pos[0] += CUBIE_SIZE;
        if (i % 4 == 2 || i % 4 == 3)
            c->verts[i].pos[1] -= CUBIE_SIZE;
        if (i > 3)
            c->verts[i].pos[2] -= CUBIE_SIZE;
    }

    for (int i = 0; i < c->colface_cnt; i++) {
        for (int j = 0; j < 4; j++) {
            glm_vec3_copy(COLORS[c->colored_faces[i]], c->verts[8 + i * 4 + j].color);
            
            glm_vec3_add(COLORED_FACE_COORDS[(pos[i] % 3) * 4 + j], c->pos, c->verts[8 + i * 4 + j].pos);
            if (pos[i] == D)
                glm_vec3_add(c->verts[8 + i * 4 + j].pos, (vec3){0.f, -(CUBIE_SIZE + 2 * OFFSET), 0.f}, c->verts[8 + i * 4 + j].pos);
            if (pos[i] == R)
                glm_vec3_add(c->verts[8 + i * 4 + j].pos, (vec3){ (CUBIE_SIZE + 2 * OFFSET), 0.f, 0.f}, c->verts[8 + i * 4 + j].pos);
            if (pos[i] == B)
                glm_vec3_add(c->verts[8 + i * 4 + j].pos, (vec3){0.f, 0.f, -(CUBIE_SIZE + 2 * OFFSET)}, c->verts[8 + i * 4 + j].pos);
        }
    }

    GLuint VBO, EBO;

    glGenVertexArrays(1, &(c->VAO));
    glBindVertexArray(c->VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * c->verts_cnt, c->verts, GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * c->inds_cnt , INDICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

cubie_t *cubie_new(vec3 pos, faces_t *colored_faces, unsigned short colface_cnt) {
    cubie_t *c = (cubie_t *) malloc(sizeof(cubie_t));
    
    glm_vec3_copy(pos, c->pos);
    c->colored_faces = colored_faces;
    c->colface_cnt = colface_cnt;

    cubie_calcverts(c, colored_faces);

    return c;
}

void cubie_draw(cubie_t c, mat4 mmodel, unsigned int modelPos) {
    glUniformMatrix4fv(modelPos, 1, GL_FALSE, &(mmodel[0][0]));
    glBindVertexArray(c.VAO);
    glDrawElements(GL_TRIANGLES, c.inds_cnt, GL_UNSIGNED_SHORT, 0);
}

void cubie_delete(cubie_t *c) {
    free(c->colored_faces);
    free(c->verts);
    glDeleteVertexArrays(1, &c->VAO);
}

cube_t *cube_new(vec3 origin, unsigned int shader) {
    cube_t *c = (cube_t *) malloc(sizeof(cube_t));
    glm_vec3_copy(origin, c->origin);

    for (unsigned short i = 0; i < CUBIE_COUNT; i++) {
        // ***Coordinates
        // Set Top Left Corner as base
        glm_vec3_copy((vec3){-  (float)CUBE_LENGTH * CUBIE_SIZE / 2.0f + origin[0],
                                (float)CUBE_LENGTH * CUBIE_SIZE / 2.0f + origin[1],
                                (float)CUBE_LENGTH * CUBIE_SIZE / 2.0f + origin[2]}, c->c[i].pos);
        // X - Offset
        glm_vec3_add(c->c[i].pos, (vec3){(i % CUBE_LENGTH) * CUBIE_SIZE, .0f, .0f}, c->c[i].pos);
        // Y - Offset
        glm_vec3_add(c->c[i].pos, (vec3){.0f, -(((i / CUBE_LENGTH) % CUBE_LENGTH) * CUBIE_SIZE), .0f}, c->c[i].pos);
        // Z - Offset
        glm_vec3_add(c->c[i].pos, (vec3){.0f, .0f, -((i / (CUBE_LENGTH * CUBE_LENGTH)) * CUBIE_SIZE)}, c->c[i].pos);

        // ***Colors
        c->c[i].colface_cnt = (CUBE_IS_UP(i) || CUBE_IS_DOWN(i)) + (CUBE_IS_LEFT(i) || CUBE_IS_RIGHT(i)) + (CUBE_IS_FRONT(i) || CUBE_IS_BACK(i));
        c->c[i].colored_faces = (faces_t *)malloc(sizeof(faces_t) * (c->c[i].colface_cnt));

        unsigned char col[5] = {0};
        for (unsigned short k = 0; k < c->c[i].colface_cnt; k++) {
            // Up
            if (CUBE_IS_UP(i) && !col[0]) {
                c->c[i].colored_faces[k] = U;
                col[0] = 1;
            }
            // Left
            else if (CUBE_IS_LEFT(i) && !col[1]) {
                c->c[i].colored_faces[k] = L;
                col[1] = 1;
            }
            // Front
            else if (CUBE_IS_FRONT(i) && !col[2]) {
                c->c[i].colored_faces[k] = F;
                col[2] = 1;
            }
            // Down
            else if (CUBE_IS_DOWN(i) && !col[3]) {
                c->c[i].colored_faces[k] = D;
                col[3] = 1;
            }
            // Right
            else if (CUBE_IS_RIGHT(i) && !col[4]) {
                c->c[i].colored_faces[k] = R;
                col[4] = 1;
            }
            // Back
            else if (CUBE_IS_BACK(i)) {
                c->c[i].colored_faces[k] = B;
            }
        }
        cubie_calcverts(&c->c[i], c->c[i].colored_faces);
        glm_mat4_identity(c->c[i].mmodel);
    }
    c->mmodel_loc = glGetUniformLocation(shader, "model");

    for (unsigned short k = 0; k < CUBIES_PER_FACE; k++)
        c->rot_indices[k] = CUBIE_COUNT + 1;
    
    c->last_rot = 0.0;
    c->cur_step = STEPS_PER_HALF;
    c->rot_step = 0.f;
    c->is_rotating = 0;
    c->algorithm = NULL;
    c->algo_len = 0;
    return c;
}

void cube_execute_move(cube_t *c, moves_t move) {
    if (c->is_rotating) return;

    vec3 invAxis;
    unsigned short swap_inds[4][CUBE_LENGTH];
    cubie_t tmp[CUBIE_COUNT];
    memcpy(&tmp[0], &c->c, CUBIE_COUNT * sizeof(cubie_t));

    switch(move) {
        case Ux1:
        case Ux2:
        case Ux3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_UP(i)) {
                    c->rot_indices[j++] = i;
                    
                    if (CUBE_IS_FRONT(i))  swap_inds[0][l++] = i;
                    if (CUBE_IS_LEFT(i))   swap_inds[1][CUBE_LENGTH - ++k] = i;
                    if (CUBE_IS_BACK(i))   swap_inds[2][CUBE_LENGTH - ++n] = i;
                    if (CUBE_IS_RIGHT(i))  swap_inds[3][m++] = i;
                }
            }
            glm_vec3_negate_to(Y_AXIS, invAxis);
            glm_vec3_copy(invAxis, c->rotAxis);
            break;

        case Lx1:
        case Lx2:
        case Lx3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_LEFT(i)) {
                    c->rot_indices[j++] = i;

                    if (CUBE_IS_UP(i))      swap_inds[0][k++] = i;
                    if (CUBE_IS_FRONT(i))   swap_inds[1][CUBE_LENGTH - ++l] = i;
                    if (CUBE_IS_DOWN(i))    swap_inds[2][CUBE_LENGTH - ++m] = i;
                    if (CUBE_IS_BACK(i))    swap_inds[3][n++] = i;
                }
            }
            glm_vec3_copy(X_AXIS, c->rotAxis);
            break;

        case Fx1:
        case Fx2:
        case Fx3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_FRONT(i)) {
                    c->rot_indices[j++] = i;

                    if (CUBE_IS_LEFT(i))    swap_inds[0][l++] = i;
                    if (CUBE_IS_UP(i))      swap_inds[1][CUBE_LENGTH - ++k] = i;
                    if (CUBE_IS_RIGHT(i))   swap_inds[2][CUBE_LENGTH - ++n] = i;
                    if (CUBE_IS_DOWN(i))    swap_inds[3][m++] = i;
                }
            }
            glm_vec3_negate_to(Z_AXIS, invAxis);
            glm_vec3_copy(invAxis, c->rotAxis);
            break;

        case Dx1:
        case Dx2:
        case Dx3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_DOWN(i)) {
                    c->rot_indices[j++] = i;

                    if (CUBE_IS_LEFT(i))   swap_inds[0][k++] = i;
                    if (CUBE_IS_FRONT(i))  swap_inds[1][CUBE_LENGTH - ++l] = i;
                    if (CUBE_IS_RIGHT(i))  swap_inds[2][CUBE_LENGTH - ++m] = i;
                    if (CUBE_IS_BACK(i))   swap_inds[3][n++] = i;
                }
            }
            glm_vec3_copy(Y_AXIS, c->rotAxis);
            break;

        case Rx1:
        case Rx2:
        case Rx3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_RIGHT(i)) {
                    c->rot_indices[j++] = i;

                    if (CUBE_IS_FRONT(i))   swap_inds[0][k++] = i;
                    if (CUBE_IS_UP(i))      swap_inds[1][CUBE_LENGTH - ++l] = i;
                    if (CUBE_IS_BACK(i))    swap_inds[2][CUBE_LENGTH - ++m] = i;
                    if (CUBE_IS_DOWN(i))    swap_inds[3][n++] = i;
                }
            }
            glm_vec3_negate_to(X_AXIS, invAxis);
            glm_vec3_copy(invAxis, c->rotAxis);
            break;

        case Bx1:
        case Bx2:
        case Bx3:
            for (unsigned short i = 0, j = 0, k = 0, l = 0, m = 0, n = 0; i < CUBIE_COUNT; i++) {
                if (CUBE_IS_BACK(i)) {
                    c->rot_indices[j++] = i;

                    if (CUBE_IS_UP(i))      swap_inds[0][k++] = i;
                    if (CUBE_IS_LEFT(i))    swap_inds[1][CUBE_LENGTH - ++l] = i;
                    if (CUBE_IS_DOWN(i))    swap_inds[2][CUBE_LENGTH - ++m] = i;
                    if (CUBE_IS_RIGHT(i))   swap_inds[3][n++] = i;
                }
            }
            glm_vec3_copy(Z_AXIS, c->rotAxis);
            break;
    }
    for (unsigned short k = 0; k < ((move / 6) + 1); k++) {
        for (unsigned short i = 0; i < 4; i++) {
            for (unsigned short j = 0; j < CUBE_LENGTH; j++) {
                tmp[swap_inds[(i + 1) % 4][j]] = c->c[swap_inds[i][j]];
            }
        }
        memcpy(&c->c, &tmp, CUBIE_COUNT * sizeof(cubie_t));
    }
    c->cur_step = 0;
    c->rot_step = ((move / 6 == 0 || move / 6 == 1) - (move / 6 == 2)) * ROTATION_FACTOR;
    c->rot_step = glm_rad(c->rot_step);
    c->is_rotating = (move / 6 == 1) + 1;
}

void cube_draw(cube_t *c) {
    // Check if we should apply a new move
    if (!c->is_rotating && c->algo_len > 0) {
        cube_execute_move(c, c->algorithm[0]);

        // Remove first item of algorithm
        for(int i = 0; i < c->algo_len - 1; i++) {
            *(c->algorithm+i) = *(c->algorithm+i+1);
        }
        c->algorithm = (moves_t *)realloc(c->algorithm, (c->algo_len-1)*sizeof(moves_t));
        // decrement algorithm length
        c->algo_len--;
    }


    // Check if we should rotate the cube
    double acttime = glfwGetTime();
    if (acttime - c->last_rot >= ANIMATION_INTERVALL) {
        c->last_rot = acttime;

        if (c->cur_step < (unsigned short)(c->is_rotating == 2 ? STEPS_PER_HALF : STEPS_PER_QUARTER)) {
            for (unsigned short i = 0; i < CUBIES_PER_FACE; i++) {
                CGLM_ALIGN_MAT mat4 rot;
                glm_rotate_make(rot, c->rot_step, c->rotAxis);
                glm_mul_rot(rot, c->c[c->rot_indices[i]].mmodel, c->c[c->rot_indices[i]].mmodel);
            }
            c->cur_step++;
        } else
            c->is_rotating = 0;
    }

    // Draw each cubie of the cube
    for (unsigned short i = 0, j = 0; i < CUBIE_COUNT; i++) {
        cubie_draw(c->c[i], c->c[i].mmodel, c->mmodel_loc);
    }
}

cube_t *cube_from_face_cube(vec3 origin, const char *face_cube, unsigned int shader) {
    cube_t *c = cube_new(origin, shader);
    // 6 faces each CUBIES_PER_FACE - faces
    faces_t faces[CUBIES_PER_FACE * 6];
    // Convert string to faces
    for (unsigned short i = 0; i < CUBIES_PER_FACE * 6; i++) {
        switch (face_cube[i]) {
            case 'U':
                faces[i] = U;
            break;
            case 'L':
                faces[i] = L;
            break;
            case 'F':
                faces[i] = F;
            break;
            case 'D':
                faces[i] = D;
            break;
            case 'R':
                faces[i] = R;
            break;
            case 'B':
                faces[i] = B;
            break;
        }
    }

    // Offset of the different cube faces
    unsigned short inds[6] = { 0, 4 * CUBIES_PER_FACE, 2 * CUBIES_PER_FACE, 3 * CUBIES_PER_FACE, CUBIES_PER_FACE, 5 * CUBIES_PER_FACE };
    for (unsigned short i = 0; i < CUBIE_COUNT; i++) {
        unsigned char col[5] = {0};
        faces_t pos[c->c[i].colface_cnt];
        for (unsigned short k = 0; k < c->c[i].colface_cnt; k++) {
            // Up
            if (CUBE_IS_UP(i) && !col[0]) {
                c->c[i].colored_faces[k] = faces[(CUBE_LENGTH - 1 - inds[0] / CUBE_LENGTH) * CUBE_LENGTH + (inds[0] % CUBE_LENGTH)];
                inds[0]++;
                col[0] = 1;
                pos[k] = U;
            }
            // Left
            else if (CUBE_IS_LEFT(i) && !col[1]) {
                c->c[i].colored_faces[k] = faces[(CUBE_LENGTH - (inds[1] % CUBIES_PER_FACE) / CUBE_LENGTH - 1) + (inds[1] % CUBE_LENGTH) * CUBE_LENGTH + 4 * CUBIES_PER_FACE];
                inds[1]++;
                col[1] = 1;
                pos[k] = L;
            }
            // Front
            else if (CUBE_IS_FRONT(i) && !col[2]) {
                c->c[i].colored_faces[k] = faces[inds[2]++];
                col[2] = 1;
                pos[k] = F;
            }
            // Down
            else if (CUBE_IS_DOWN(i) && !col[3]) {
                c->c[i].colored_faces[k] = faces[inds[3]++];
                col[3] = 1;
                pos[k] = D;
            }
            // Right
            else if (CUBE_IS_RIGHT(i) && !col[4]) {
                c->c[i].colored_faces[k] = faces[((inds[4] % CUBIES_PER_FACE) / CUBE_LENGTH) + (inds[4] % CUBE_LENGTH) * CUBE_LENGTH + CUBIES_PER_FACE];
                inds[4]++;
                col[4] = 1;
                pos[k] = R;
            }
            // Back
            else if (CUBE_IS_BACK(i)) {
                c->c[i].colored_faces[k] = faces[((inds[5] % CUBIES_PER_FACE) / CUBE_LENGTH) * CUBE_LENGTH + (CUBE_LENGTH - inds[5] % CUBE_LENGTH - 1) + 5 * CUBIES_PER_FACE];
                inds[5]++;
                pos[k] = B;
            }
        }
        // calculate new cubie vertices
        cubie_calcverts(&c->c[i], &pos[0]);
    }
    return c;
}

void cube_execute_algorithm(cube_t *c, char *algorithm) {
    int str_len = strlen(algorithm);
    int move_count = 0;

    // Count number of moves
    for (int i = 0; i < str_len; i++) {
        if (algorithm[i] == '\'' || algorithm[i] == '2' ) continue;
        move_count++;
    }
    c->algo_len = move_count;
    // Allocate memory
    if (!c->algorithm)  c->algorithm = (moves_t *) malloc(move_count * sizeof(moves_t));
    else                c->algorithm = (moves_t *) realloc(c->algorithm, move_count * sizeof(moves_t));

    // Assign moves to algorithm
    for (int i = 0, j = 0; i < move_count; i++) {
        
        switch (algorithm[j++]) {
            case 'U':
                c->algorithm[i] = Ux1;
            break;
            case 'L':
                c->algorithm[i] = Lx1;
            break;
            case 'F':
                c->algorithm[i] = Fx1;
            break;
            case 'D':
                c->algorithm[i] = Dx1;
            break;
            case 'R':
                c->algorithm[i] = Rx1;
            break;
            case 'B':
                c->algorithm[i] = Bx1;
            break;
        }

        if (algorithm[j] == '2') {
            c->algorithm[i] += 6;
            j++;
        }
        else if (algorithm[j] == '\'') {
            c->algorithm[i] += 12;
            j++;
        }
    }
}

void cube_delete(cube_t *c) {
    for (int i = 0; i < CUBIE_COUNT; i++) {
        cubie_delete(&c->c[i]);
    }
}