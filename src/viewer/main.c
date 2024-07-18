#include "../paws_data.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

#define APP_TITLE "Kitty Paws"

/**
 * @brief Allocate memory for each member of paws_mesh object
 * @param[out] mesh a paws_mesh object
 * @return status: false:OK true:ERROR
 * @version 0.1.0
 */
bool paws_mesh_ctor(paws_mesh* mesh) {
    bool status = false;

    mesh->vertices = cvector_new(1);
    mesh->textures = cvector_new(1);
    mesh->normals = cvector_new(1);
    mesh->faces = cvector_new(1);

    if ( !mesh->vertices || 
         !mesh->textures || 
         !mesh->normals ||
         !mesh->faces
    ) {
        status = true;
    }

    return status;
}

/**
 * @brief Free allocated memory for each member of paws_mesh object
 * @param[out] mesh a paws_mesh object
 * @version 0.1.0
 */
void paws_mesh_dtor(paws_mesh* mesh) {
    for (size_t i = 0; i < cvector_size(mesh->faces); ++i) {
        cvector* face_indeces = cvector_at(mesh->faces, i);

        if ( !face_indeces ) {
            continue;
        }

        for (size_t j = 0; j < cvector_size(face_indeces); ++j) {
            paws_face_indices* obj = cvector_at(face_indeces, j);

            if ( obj ) {
                free(obj);
            }
        }

        cvector_delete(face_indeces);
    }

    cvector_delete(mesh->vertices);
    cvector_delete(mesh->textures);
    cvector_delete(mesh->normals);
    cvector_delete(mesh->faces);
}


int main() {
    int status = 0;

    int screen_width = 900;
    int screen_height = 900;

    // TODO: create struct with filepath, mesh, something more to hold object
    // - Also write functions for manage this struct (load, save, unload, etc)

    // hardcoded value for testing
    char* mesh_filepath = "/home/konnor/code/c/graphics/3d_objects/cube/cube.obj";
    paws_mesh mesh = {0};
    bool is_mesh_loaded = false;

    if ( paws_mesh_ctor(&mesh) ) {
        status = 1;
        fprintf(stderr, "[ERROR] Can't Allocate memory for mesh\n");
    }

    if ( !parse_format_obj(mesh_filepath, &mesh) ) {
        is_mesh_loaded = true;
    }

    Color color_background = WHITE;

    // SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_TITLE);

    Camera camera = {
        .position = {.x = 0, .y = 1, .z = 4},
        .target = {.x = 0, .y = 0, .z = 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .fovy = 45,
        .projection = CAMERA_PERSPECTIVE,
    };

    while ( !status && !WindowShouldClose() ) {
        if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        }

        BeginDrawing();
        {
            ClearBackground(color_background);
            BeginMode3D(camera);
            {
                DrawGrid(15, 10);

                if ( is_mesh_loaded ) {
                    draw_mesh(&mesh);
                }
            }
            EndMode3D();
        }
        EndDrawing();

    }

    CloseWindow();

    paws_mesh_dtor(&mesh);

    return status;
}
