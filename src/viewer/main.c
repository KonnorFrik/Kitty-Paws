#include "../paws_data.h"
#include <raylib.h>
#include <stdio.h>

#define APP_TITLE "Kitty Paws"

int main() {
    // Initialize anything here
    int status = 0;

    int screen_width = 900;
    int screen_height = 900;
    int target_fps = 60;

    // hardcoded value for testing
    char* mesh_filepath = "/home/konnor/code/c/graphics/3d_objects/cube/cube.obj";
    paws_mesh mesh = {0};

    if ( paws_mesh_ctor(&mesh) ) {
        status = 1;
        fprintf(stderr, "[ERROR] Can't Allocate memory for mesh\n");
    }

    if ( parse_format_obj(mesh_filepath, &mesh) ) {
        // error placeholdor
    }

    Color color_background = WHITE;

    // SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_TITLE);
    SetTargetFPS(target_fps);

    Camera camera = {
        .position = {.x = 0, .y = 1, .z = 4},
        .target = {.x = 0, .y = 0, .z = 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .fovy = 45,
        .projection = CAMERA_PERSPECTIVE,
    };

    while ( !status && !WindowShouldClose() ) {
        // Update anything here
        if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        }

        // Draw anything here
        BeginDrawing();
        {
            ClearBackground(color_background);
            BeginMode3D(camera);
            {
                // DrawGrid(15, 10);

                if ( mesh.is_loaded ) {
                    draw_mesh(&mesh);
                }
            }
            EndMode3D();
        }
        EndDrawing();

    }

    // Deinitialize anything here
    CloseWindow();
    paws_mesh_dtor(&mesh);

    return status;
}
